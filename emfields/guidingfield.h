#ifndef GUIDINGFIELD_H
#define GUIDINGFIELD_H

namespace GuidingFields{

	struct GuidingField{
		Matrix<double,3,1> B, A, Adag, phi_grad, b, Bdag, B_grad;
		Matrix<double,3,3> Adag_jac;
		double Bnorm, phi;
	};

	class GuidingFieldConfiguration
	{
		private:
			Vector3d B_grad(Vector3d x);
		public:
			GuidingFieldConfiguration(Config::Config* config): mu(2.25E-6), hx(1.E-5){};
			~GuidingFieldConfiguration(){};

			GuidingField compute(Matrix<double,4,1> q);
			virtual Vector3d guiding_A(Vector3d x) = 0;
			virtual Vector3d guiding_B(Vector3d x) = 0;

			Matrix<double,3,3> B_hessian(Vector3d x);

			const double mu;
			const double hx;  //step for numerical derivative
	};


	Vector3d GuidingFieldConfiguration::B_grad(Vector3d x){
		Vector3d dx(hx,hx,hx); //dx := (dx,dy,dz)
		Vector3d ret;
		Vector3d x0,x1;
		for (int j=0;j<3;j++){
		  x0 = x1 = x;
		  x0(j)-=dx(j);
		  x1(j)+=dx(j);
		  ret(j) = 0.5*(guiding_B(x1).norm() - guiding_B(x0).norm())/dx(j);
		}
		return ret;
	}
	
	Matrix<double,3,3> GuidingFieldConfiguration::B_hessian(Vector3d x){
		// COMPUTE B_HESSIAN
		Vector3d dx(hx,hx,hx); //dx := (dx,dy,dz)
		Matrix<double,3,3> ret;
		Vector3d x0,x1;
		for (int j=0;j<3;j++){
		  x0 = x1 = x;
		  x0(j)-=dx(j);
		  x1(j)+=dx(j);
		  ret.col(j) = 0.5*(B_grad(x1) - B_grad(x0))/dx(j);
		}

		return ret;
	}

	GuidingField GuidingFieldConfiguration::compute(Matrix<double,4,1> z){
		//TODO: implement phi
		BOOST_LOG_TRIVIAL(trace) << "Computing Magnetic field";
		BOOST_LOG_TRIVIAL(trace) << std::scientific << "z:\t\t" << z.transpose();
		
		GuidingField field;
		Vector3d x = z.head(3);
		double u = z(3);

		field.A = guiding_A(x);
		field.B = guiding_B(x);
		field.Bnorm = field.B.norm();
		field.b = field.B.normalized();
		field.Adag = field.A + u*field.b;
		
		//COMPUTE SPACESTEP
		Vector3d B0,x0,x1,B1;
		Vector3d dx(hx,hx,hx); //dx := (dx,dy,dz)

		//COMPUTE GRADIENT(B),GRADIENT(phi),JAC(A_dagger)
		Matrix3d A_jac;
		Matrix3d b_jac;
		for (int j=0;j<3;j++){
		  x0 = x1 = x;
		  x0(j)-=dx(j);
		  x1(j)+=dx(j);
		  B0 = guiding_B(x0);
		  B1 = guiding_B(x1);
		  
		  field.Adag_jac.col(j) = 0.5*(guiding_A(x1) + u*B1.normalized() - guiding_A(x0)-u*B0.normalized())/dx(j);
		  A_jac.col(j) = 0.5*(guiding_A(x1) - guiding_A(x0))/dx(j);
		  field.B_grad(j) = 0.5*(B1.norm() - B0.norm())/dx(j);
		  b_jac.col(j) = 0.5*(B1.normalized() - B0.normalized())/dx(j);
		}

		//COMPUTE B_dagger
		field.Bdag = field.B;
		field.Bdag(0) += u*(b_jac(2,1)-b_jac(1,2));
		field.Bdag(1) += u*(b_jac(0,2)-b_jac(2,0));
		field.Bdag(2) += u*(b_jac(1,0)-b_jac(0,1));		

		BOOST_LOG_TRIVIAL(trace) << std::scientific << "B:\t\t" << field.B.transpose();
		BOOST_LOG_TRIVIAL(trace) << std::scientific << "A:\t\t" << field.A.transpose();
		return field;
	}
}

#endif