/*
 * geometry.cpp
 *
 *  Created on: Nov 9, 2012
 *      Author: krigu
 */

#include <vector>
#include "geometry.hpp"
#include "../exception/GazeExceptions.hpp"

const double Rad2Deg = 180.0 / 3.1415;
const double Deg2Rad = 3.1415 / 180.0;

int calcPointDistance(cv::Point *point1, cv::Point *point2) {
	int distX = point1->x - point2->x;
	int distY = point1->y - point2->y;
	int sum = distX * distX + distY * distY;
    
	return sqrt(sum);
}

cv::Point calcRectBarycenter(cv::Rect& rect) {
	int x = rect.width / 2;
	int y = rect.height / 2;

	return cv::Point(rect.x + x, rect.y + y);
}

/// <summary>
/// Calculates angle in radians between two points and x-axis.
/// </summary>
double calcAngle(cv::Point start, cv::Point end)
{
	return atan2(start.y - end.y, end.x - start.x) * Rad2Deg;
}

cv::Point2f calcAverage(std::vector<cv::Point2f> points){
	int amount = points.size();

	if (amount == 0){
        throw new WrongArgumentException("Cannot calc the average of 0 Points!");
	}

	int sumX = 0;
	int sumY = 0;
	for (std::vector<cv::Point2f>::iterator it = points.begin(); it != points.end();
			++it) {
		sumX += it->x;
		sumY += it->y;
	}

	float x = sumX / amount;
	float y = sumY / amount;

	return cv::Point2f(x,y);
}

/**
 * 
 * original matlab code:
 * 
 *  N = size(points,1);
 *  x = points(:,1);
 *  y = points(:,2);
 *  M = [2*x 2*y ones(N,1)];
 *  v = x.^2+y.^2;
 *  pars = M\v;
 *  x0 = pars(1);
 *  y0 = pars(2);
 *  center = pars(1:2);
 *  R = sqrt(sum(center.^2)+pars(3));
 *  if imag(R)~=0,
 *      error('Imaginary squared radius computed');
 *  end;
 * 
 * @param x
 * @param y
 * @param radius
 * @param pointsToFit
 */
void bestFitCircle(float * x, float * y, float * radius,
        std::vector<cv::Point2f> pointsToFit) {

    unsigned int numPoints = pointsToFit.size();
    
    //setup an opencv mat with our points
    // col 1 = x coordinates and col 2 = y coordinates
    cv::Mat1f points(numPoints, 2);

    unsigned int i=0;
    for (std::vector<cv::Point2f>::iterator it = pointsToFit.begin(); it != pointsToFit.end(); ++it) {
        points.at<float>(i,0) = it->x;
        points.at<float>(i,1) = it->y;
        ++i;
    }

    cv::Mat1f x_coords = points.col(0);
    cv::Mat1f y_coords = points.col(1);
    cv::Mat1f ones = cv::Mat::ones(numPoints, 1, CV_32F);
    
    // calculate the M matrix
    cv::Mat1f M(numPoints, 3);
    cv::Mat1f x2 = x_coords * 2;
    cv::Mat1f y2 = y_coords * 2;
    x2.col(0).copyTo(M.col(0));
    y2.col(0).copyTo(M.col(1));
    ones.col(0).copyTo(M.col(2));
    
    // calculate v
    cv::Mat1f v = x_coords.mul(x_coords) + y_coords.mul(y_coords);
    
    //TODO some error handling in solve()?
    cv::Mat pars;
    cv::solve(M, v, pars, cv::DECOMP_SVD);
    
    *x = pars.at<float>(0,0);
    *y = pars.at<float>(0,1);
    *radius = sqrt(pow(*x,2) + pow(*y,2) + pars.at<float>(0,2)); 
}
