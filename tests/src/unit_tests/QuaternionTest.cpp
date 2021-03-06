#include <gmock/gmock-matchers.h>
#include "EulerAngles.h"

using namespace testing;
using namespace flash::math;

float const DELTA = 0.006;
float const EA_DELTA = 0.03;

class QuaternionTest : public testing::Test {
public:
	Quaternion quaternion = Quaternion();

	void SetUp() override {
		reset();
	}

	void set(float w, float x, float y, float z) {
		quaternion.w(w);
		quaternion.x(x);
		quaternion.y(y);
		quaternion.z(z);
	}

	void setAxisAngle(float angle, float vx, float vy, float vz) {
		float toRadians = (float) (M_PI / 180);
		quaternion = Quaternion(angle * toRadians, Vec4(vx, vy, vz));
	}

	void reset() {
		quaternion.w(1);
		quaternion.x(0);
		quaternion.y(0);
		quaternion.z(0);
	}

	void assertClose(const Quaternion q, float w, float x, float y, float z) {
		ASSERT_THAT(q.w(), FloatNear(w, DELTA));
		ASSERT_THAT(q.x(), FloatNear(x, DELTA));
		ASSERT_THAT(q.y(), FloatNear(y, DELTA));
		ASSERT_THAT(q.z(), FloatNear(z, DELTA));
	}

	void assertClose(Mat4 matrix, float x1, float y1, float z1, float x2, float y2, float z2, float x3,
			float y3, float z3) {
		ASSERT_THAT(matrix.x1(), FloatNear(x1, DELTA));
		ASSERT_THAT(matrix.y1(), FloatNear(y1, DELTA));
		ASSERT_THAT(matrix.z1(), FloatNear(z1, DELTA));
		ASSERT_THAT(matrix.x2(), FloatNear(x2, DELTA));
		ASSERT_THAT(matrix.y2(), FloatNear(y2, DELTA));
		ASSERT_THAT(matrix.z2(), FloatNear(z2, DELTA));
		ASSERT_THAT(matrix.x3(), FloatNear(x3, DELTA));
		ASSERT_THAT(matrix.y3(), FloatNear(y3, DELTA));
		ASSERT_THAT(matrix.z3(), FloatNear(z3, DELTA));
	}

	void assertMatrix(float x1, float y1, float z1, float x2, float y2, float z2, float x3,
			float y3, float z3) {
		assertClose(quaternion.toMatrix(), x1, y1, z1, x2, y2, z2, x3, y3, z3);
	}
};

TEST_F(QuaternionTest, VectorConstructor) {
	setAxisAngle(30, 1, 0, 0);
	assertClose(quaternion, 0.966, 0.259, 0, 0);
}

TEST_F(QuaternionTest, ToEulerAngles) {
	set(0.966, 0.259, 0, 0);
	EulerAngles ea = quaternion.uprightToEulerAngles();
	ASSERT_THAT(ea.heading(), FloatNear(0, EA_DELTA));
	ASSERT_THAT(ea.pitch(), FloatNear(30, EA_DELTA));
	ASSERT_THAT(ea.bank(), FloatNear(0, EA_DELTA));
}

TEST_F(QuaternionTest, UprightQuaternionFromEulerAngles) {
	set(0.966, 0.259, 0, 0);
	EulerAngles ea = quaternion.uprightToEulerAngles();
	Quaternion q = ea.toUprightQuaternion();
	assertClose(q, 0.966, 0.259, 0, 0);
}

TEST_F(QuaternionTest, ObjectQuaternionFromEulerAngles) {
	set(0.966, 0.259, 0, 0);
	EulerAngles ea = quaternion.objectToEulerAngles();
	Quaternion q = ea.toObjectQuaternion();
	assertClose(q, 0.966, 0.259, 0, 0);
}

TEST_F(QuaternionTest, ToMatrix) {
	setAxisAngle(98.4, -0.863f, -0.357f, -0.357f);
	assertMatrix(0.707, 0, 0.707, 0.707, 0, -0.707f, 0, 1, 0);

	setAxisAngle(0, 0.707, -0.707f, 0);
	assertMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);

	setAxisAngle(87, 0.089, -0.506f, 0.857);
	assertMatrix(0.061, 0.814, 0.578, -0.9f, 0.296, -0.322f, -0.433f, -0.5f, 0.75);

	setAxisAngle(135, 1, 0, 0);
	assertMatrix(1, 0, 0, 0, -0.707f, 0.707, 0, -0.707f, -0.707f);

	setAxisAngle(261.6, 0.863, 0.357, 0.357);
	assertMatrix(0.707, 0, 0.707, 0.707, 0, -0.707f, 0, 1, 0);

	setAxisAngle(139, -0.156f, -0.912f, -0.378f);
	assertMatrix(-0.707f, 0, 0.707, 0.5, 0.707, 0.5, -0.5f, 0.707, -0.5f);

	setAxisAngle(7200, 0, -1, 0);
	assertMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);

	setAxisAngle(-135, -1, 0, 0);
	assertMatrix(1, 0, 0, 0, -0.707f, 0.707, 0, -0.707f, -0.707f);
}

TEST_F(QuaternionTest, FromMatrix) {
	setAxisAngle(98.4, -0.863f, -0.357f, -0.357f);
	const Mat4 matrix = quaternion.toMatrix();
	const Quaternion q = Quaternion::fromMatrix(matrix);
	assertClose(q, q.w(), q.x(), q.y(), q.z());
}

TEST_F(QuaternionTest, Multiplication) {
	quaternion = Quaternion(0.965, 0.149, -0.149f, 0.149);
	quaternion = quaternion * quaternion;
	assertClose(quaternion, 0.867, 0.288, -0.288f, 0.288);

	quaternion = Quaternion(0.233, 0.06, -0.257f, -0.935f);
	Quaternion quaternion2 = Quaternion(-0.752f, 0.286, 0.374, 0.459);
	Quaternion resultQuaternion = quaternion * quaternion2;
	assertClose(resultQuaternion, 0.333, 0.253, -0.015f, 0.906);
}

TEST_F(QuaternionTest, DotProduct) {
	quaternion = Quaternion(0.233, 0.06, -0.257f, -0.935f);
	Quaternion quaternion2 = Quaternion(-0.752f, 0.286, 0.374, 0.459);

	float dotProduct = Quaternion::dotProduct(quaternion, quaternion2);
	ASSERT_THAT(dotProduct, FloatNear(-0.683, DELTA));
}

TEST_F(QuaternionTest, Difference) {
	quaternion = Quaternion(0.233, 0.06, -0.257f, -0.935f);
	Quaternion quaternion2 = Quaternion(-0.752f, 0.286, 0.374, 0.459);

	Quaternion resultQuaternion = Quaternion::getDifference(quaternion, quaternion2);
	assertClose(resultQuaternion, -0.683f, 0.343, -0.401f, -0.5f);
}

TEST_F(QuaternionTest, Exponentiation) {
	setAxisAngle(90, 1, 0, 0);

	float exponent = 0.33333333f;
	quaternion = quaternion.exp(exponent);
	
	assertClose(quaternion, 0.9659, 0.2588, 0, 0);
}

TEST_F(QuaternionTest, Slerp) {
	setAxisAngle(20, 1, 0, 0);
	Quaternion start = quaternion;
	setAxisAngle(40, 1, 0, 0);
	float fraction = 0.5;
	quaternion = start.slerp(quaternion, fraction);

	assertClose(quaternion, 0.9659, 0.2588, 0, 0);
}