#ifndef IMAGE_REC_LEVEL_PARSER_HPP_
#define IMAGE_REC_LEVEL_PARSER_HPP_

#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <memory>

#ifdef OPENCV_LIBS
#include <opencv2/opencv.hpp>
#endif

#include <string>
#include <vector>

#include "Level.hpp"
#include "Array2D.hpp"

using namespace std;

#ifdef OPENCV_LIBS
using namespace cv;
#endif

class ImageRecLevelParser {
private:
    string filePath;
    constexpr static float DEG2RAD = glm::pi<float>() / 180.f;
#ifdef OPENCV_LIBS
    const Vec3b COLOR_OBSTACLE = Vec3b(0, 0, 0);  // BLACK
    const Vec3b COLOR_INFLOW = Vec3b(255, 0, 0);  // BLUE
    const Vec3b COLOR_OUTFLOW = Vec3b(0, 0, 255); // RED
    const Vec3b COLOR_BALL = Vec3b(0, 255, 0); // GREEN
#endif

public:
    ImageRecLevelParser(string filePath) : filePath(filePath) {}

    bool parse(Level &level) {
#ifndef OPENCV_LIBS
        return false;
#else
        Mat levelImg = imread(filePath);
        if (!levelImg.data) {
            return false;
        }

        float radius;
        Point2f center;
        Size levelSize = levelImg.size();
        Mat noBorderImg, boxPoints2f, boxPoints;
        vector<Vec4i> hierarchy;
        vector<vector<Point>> contours;

        level.width = levelSize.width;
        level.height = levelSize.height;

        Rect noBorderROI(1, 1, levelSize.width-2, levelSize.height-2);
        cvtColor(levelImg(noBorderROI), noBorderImg, COLOR_BGR2GRAY);
        threshold(noBorderImg, noBorderImg, 1, 255, THRESH_BINARY);
        findContours(noBorderImg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(1, 1));

        int i = 0;
        int rigidBodyId = 1;

        for (const vector<Point> &contourPoints : contours) {
            // Skip contour, if it contains other contours
            if (hierarchy[i++][2] != -1) continue;

            // Enclosing circle
            minEnclosingCircle(contourPoints, center, radius);
            radius -= 1.5f;

            // Enclosing Rect
            RotatedRect rect = minAreaRect(contourPoints);
            rect.size.width -= 2;
            rect.size.height -= 2;
            cv::boxPoints(rect, boxPoints2f);
            boxPoints2f.assignTo(boxPoints, CV_32S); // Convert Point2f to Point

            // Choose the one with the smaller area
            if (radius * radius * glm::pi<float>() <= rect.size.area()) {
                circle(levelImg, center, radius, Scalar(0, 0, 0), -1);
                auto rigidBody = make_unique<RigidBodyCircle>(rigidBodyId, center.x, level.height - center.y - 1, radius, 0);
                level.setUniqueMesh(rigidBodyId, rigidBody->createColoredMesh(Level::OBSTACLE_COLOR));
                level.rigidBodies.push_back(move(rigidBody));
            }
            else {
                fillConvexPoly(levelImg, boxPoints, Scalar(0, 0, 0));
                auto rigidBody = make_unique<RigidBodyRect>(rigidBodyId,
                                                            rect.center.x, level.height - rect.center.y - 1,
                                                            rect.size.width, rect.size.height,
                                                            0.f, rect.angle * DEG2RAD);
                level.setUniqueMesh(rigidBodyId, rigidBody->createColoredMesh(Level::OBSTACLE_COLOR));
                level.rigidBodies.push_back(move(rigidBody));
            }
            rigidBodyId++;
        }
        // cvtColor(noBorderImg, levelImg(noBorderROI), COLOR_GRAY2BGR);

        level.matrix = Array2D<Level::CellType>(levelSize.width, levelSize.height);
        for (int x = 0; x < levelSize.width; x++) {
            for (int y = 0; y < levelSize.width; y++) {
                Vec3b pixel = levelImg.at<Vec3b>(Point(x, y));
                if (pixel == COLOR_BALL) {
                    level.addBall(rigidBodyId++, x, y);
                }
                level.matrix.value(x, level.height - y - 1) = getCellTypeFromColor(pixel);
            }
        }

        level.flipperLeftId = rigidBodyId++;
        level.flipperRightId = rigidBodyId++;

        return true;
    }

private:
    Level::CellType getCellTypeFromColor(Vec3b color) {
        if (color == COLOR_OBSTACLE) {
            return Level::OBSTACLE;
        }
        else if (color == COLOR_INFLOW) {
            return Level::INFLOW;
        }
        else if (color == COLOR_OUTFLOW) {
            return Level::OUTFLOW;
        }
        return Level::FLUID;
#endif
    }
};

#endif
