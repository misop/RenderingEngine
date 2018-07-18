#include "stdafx.h"
#include "GLCamera.h"
#include <math.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define M_PI 3.14159265359

using namespace GL;

Camera::Camera(void) {
	color[0] = 0.9; color[1] = 0.45; color[2] = 0.0;
	width = 1;
	height = 1;
	viewport = glm::vec4(0, 0, width, height);
	reset();
	dir = glm::vec3(0, 0, 0);
	pos = glm::vec3(0, 0, 0);
}

Camera::~Camera(void)
{
}

void Camera::setFi(float newFi) {
	fi = newFi;
	if (fi >= 360.0) fi -= 360.0;
	if (fi < 0) fi += 360.0;

	update();
}

void Camera::setTheta(float newTheta) {
	theta = newTheta;
	if (theta >= 179) theta = 179;
	if (theta <= 1) theta = 1;

	update();
}

void Camera::setDist(float newDist) {
	dist = newDist;
	if (dist < 0.5)
		dist = 0.5;

	update();
}

void Camera::setWidth(float newWidth) {
	width = newWidth;
	viewport.z = width;
}

void Camera::setHeight(float newHeight) {
	height = newHeight;
	viewport.w = height;
}

void Camera::strafeHorizontal(float strafe) {
	look += right * strafe;

	update();
}

void Camera::strafeVertical(float strafe) {
	look += up * strafe;

	update();
}

void Camera::update() {
	up = glm::vec3(0, 1, 0);

	eye = glm::vec3(
		look.x + sin(M_PI / 180 * theta) * sin(M_PI / 180 * fi) * dist,
		look.y + cos(M_PI / 180 * theta) * dist,
		look.z + sin(M_PI / 180 * theta) * cos(M_PI / 180 * fi) * dist);

	glm::vec3 normal = glm::normalize(look - eye);
	right = glm::normalize(glm::cross(up, normal));
	up = glm::normalize(glm::cross(normal, right));

	calculateMatrices();
}

void Camera::calculateMatrices() {
	view = glm::lookAt(eye, look, up);
	//normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelview)));
}

void Camera::lookFromCamera(GLint mvpLoc) {
	glm::mat4 MVPmatrix = projection*view;
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVPmatrix));
}

void Camera::getCameraMatrices(GLint pLoc, GLint vLoc) {
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void Camera::getProjectionMatrix(GLint pLoc) {
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Camera::getViewMatrix(GLint vLoc) {
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void Camera::setupModelViewMatrix(GLint mvLoc) {
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void Camera::setupNormalMatrix(glm::mat4 &model, GLint nmLoc) {
	glm::mat3 NM = glm::mat3(view*model);
	NM = glm::transpose(glm::inverse(NM));
	glUniformMatrix3fv(nmLoc, 1, GL_FALSE, glm::value_ptr(NM));
}

void Camera::setupNormalMatrix(GLint nmLoc) {
	glUniformMatrix3fv(nmLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

glm::mat4 Camera::cameraModelMatrix() {
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, look);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0));

	return modelMatrix;
}

void Camera::reset() {
	fi = 0;
	theta = 90;
	dist = 700;

	eye = glm::vec3(0, 0, 100);
	look = glm::vec3(0, 0, 0);
	up = glm::vec3(0, 1, 0);
	right = glm::vec3(-1, 0, 0);

	calculateMatrices();
}

glm::vec3 Camera::getRight() {
	return right;
}

glm::vec3 Camera::getUp() {
	return up;
}

glm::vec3 Camera::getView() {
	return glm::normalize(eye - look);
}

glm::vec3 Camera::getEye() {
	return eye;
}

void Camera::mousePositionTo3D(int x_cursor, int y_cursor, GLdouble &x, GLdouble &y, GLdouble &z) {
	GLfloat winX, winY, winZ = 0;
	// obtain the Z position (not world coordinates but in range 0 - 1)
	winX = (float)x_cursor;
	winY = (float)viewport[3] - (float)y_cursor;
	glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	//when clicking out of object
	winZ = 0;//0.998555;
	// obtain the world coordinates
	//GLint sucess = gluUnProject(winX, winY, winZ, modelview, projection, viewport, &x, &y, &z);
	glm::vec3 window(winX, winY, winZ);
	glm::vec3 coor = glm::unProject(window, view, projection, viewport);
	x = coor.x;
	y = coor.y;
	z = coor.z;
}
