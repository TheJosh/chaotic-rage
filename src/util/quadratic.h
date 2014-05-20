// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
using namespace std;


/**
* Represents a exponetial + linear + constant quadratic equation
*   ax^2 + bx + c = 0
**/
class Quadratic
{
	public:
		float exp;
		float lin;
		float con;

	public:
		/**
		* Construct with a given exponential, linear and constant term
		**/
		Quadratic(float const exponential, float const linear, float const constant)
		{
			this->exp = exponential;
			this->lin = linear;
			this->con = constant;
		}

		/**
		* A blank Quadratic, which will always solve to zero
		**/
		Quadratic()
		{
			this->exp = 0.0f;
			this->lin = 0.0f;
			this->con = 0.0f;
		}

		/**
		* Solves the quadratic equation for a given value of X
		**/
		float solve(float const x)
		{
			return (x * this->exp * this->exp) + (x * this->lin) + this->con;
		}
};

