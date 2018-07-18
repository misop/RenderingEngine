#pragma once
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <GL\glew.h>
#include <glm/glm.hpp>


namespace GL {
	class Camera
	{
	public:
		glm::vec3 pos;
		glm::vec3 dir;

		glm::mat4 projection;
		glm::mat4 view;
		glm::vec4 viewport;
		glm::mat3 normalMatrix;

		/*GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];*/

		float fovy;
		float aspect;
		float width;
		float height;

		float theta;
		float fi;
		float dist;

		glm::vec3 eye;
		glm::vec3 look;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 color;

		void setFi(float newFi);
		void setTheta(float newTheta);
		void setDist(float newDist);
		void setWidth(float newWidth);
		void setHeight(float newHeight);

		void strafeHorizontal(float strafe);
		void strafeVertical(float strafe);

		void update();
		void calculateMatrices();

		void lookFromCamera(GLint mvpLoc);
		void getCameraMatrices(GLint pLoc, GLint vLoc);
		void getProjectionMatrix(GLint pLoc);
		void getViewMatrix(GLint vLoc);
		void setupNormalMatrix(glm::mat4 &model, GLint nmLoc);
		void setupModelViewMatrix(GLint mvLoc);
		void setupNormalMatrix(GLint nmLoc);
		glm::mat4 cameraModelMatrix();
		void reset();

		void mousePositionTo3D(int x_cursor, int y_cursor, GLdouble &x, GLdouble &y, GLdouble &z);

		glm::vec3 getRight();
		glm::vec3 getUp();
		glm::vec3 getView();
		glm::vec3 getEye();

		Camera(void);
		~Camera(void);
	};
}
