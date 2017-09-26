#ifndef IMAGE_REC_LEVEL_PARSER_HPP_
#define IMAGE_REC_LEVEL_PARSER_HPP_
#define PI 3.1415926535f
#define DEG2RAD PI / 180

#include <glm/vec3.hpp>
#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "Level.hpp"
#include "Log.hpp"
#include "Array2D.hpp"

using namespace std;
using namespace cv;

class ImageRecLevelParser {
private:
    string filePath;
    const Vec3b COLOR_OBSTACLE = Vec3b(0, 0, 0);  // BLACK
    const Vec3b COLOR_INFLOW = Vec3b(255, 0, 0);  // BLUE
    const Vec3b COLOR_OUTFLOW = Vec3b(0, 0, 255); // RED
    const Vec3b COLOR_BALL = Vec3b(0, 255, 0); // GREEN

public:
    ImageRecLevelParser(string filePath) : filePath(filePath) {}

    bool parse(Level &level) {
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
        int rigidBodyId = Level::BALL_ID + 1;

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
            cout << "--- " << i << endl;
            cout << center << " -- " << radius << endl;
            cout << boxPoints << endl;
            cout << radius*radius*PI << " -- " << rect.size.area() << endl;

            // Choose the one with the smaller area
            if (radius * radius * PI <= rect.size.area()) {
                circle(levelImg, center, radius, Scalar(0, 0, 0), -1);
                auto rigidBody = make_unique<RigidBodyCircle>(rigidBodyId, center.x, center.y, radius);
                level.setUniqueMesh(rigidBodyId, rigidBody->createColoredMesh());
                level.rigidBodies.push_back(move(rigidBody));
            }
            else {
                fillConvexPoly(levelImg, boxPoints, Scalar(0, 0, 0));
                auto rigidBody = make_unique<RigidBodyRect>(rigidBodyId,
                                                            rect.center.x, rect.center.y,
                                                            rect.size.width, rect.size.height,
                                                            1.f, rect.angle * DEG2RAD);
                level.setUniqueMesh(rigidBodyId, rigidBody->createColoredMesh());
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
                    auto rigidBody = make_unique<RigidBodyCircle>(Level::BALL_ID, x, level.height - y - 1);
                    level.setUniqueMesh(Level::BALL_ID, rigidBody->createColoredMesh(glm::vec3{0, 255, 255}));
                    level.rigidBodies.push_back(move(rigidBody));
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
    }
};

#endif
