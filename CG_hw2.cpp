/* Author: Saumya Saxena
CS536: Computer Graphics - Assignment 2
Date: October 26, 2018
Description: Catmull-Rom Splines
*/

/* Header Files */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <string.h>
#include <fstream>
#include <math.h>

using namespace std;

struct point
{
    double x,y,z;
};

/* Function to evaluate the binomial coefficient */
double binomialCoeff(double n, double k)
{
    double res = 1;

    if ( k > n - k )
        k = n - k;

    for (double i = 0; i < k; ++i)
    {
        res *= (n - i);
        res /= (i + 1);
    }

    return res;
}

/* Function to evaluate tangent at each point except the end points and multiply with tension*/
struct point tangentPoint(int i,struct point points[100],int n, double T)
{
    struct point tangent = {0,0,0};

    if(i != 0 && i!= n)
    {
        tangent.x = ((1.00000-T)*((points[i+1].x - points[i-1].x)/2.0));
        tangent.y = ((1.00000-T)*((points[i+1].y - points[i-1].y)/2.0));
        tangent.z = ((1.00000-T)*((points[i+1].z - points[i-1].z)/2.0));
    }
    return tangent;
}

/* Function implementing the bezier equation */
struct point bezierEquation(int n,double t, struct point points[100])
{
    struct point P;
    double x = 0,y = 0,z = 0;

        n=n-1;

        for(int i=0;i<=n;i++)
        {
            x = x + binomialCoeff(n,i) * pow((1-t),(n-i)) * pow(t,i) * points[i].x;
            y = y + binomialCoeff(n,i) * pow((1-t),(n-i)) * pow(t,i) * points[i].y;
            z = z + binomialCoeff(n,i) * pow((1-t),(n-i)) * pow(t,i) * points[i].z;
        }

        P.x = x;
        P.y = y;
        P.z = z;
        return P;
}

/* Function to evaluate bezier point from first tangent */
struct point point2(struct point points[100],int i,int n,struct point tangentPoints[100])
{
    struct point p2;

    p2.x = points[i].x + tangentPoints[i].x/3.00000;
    p2.y = points[i].y + tangentPoints[i].y/3.00000;
    p2.z = points[i].z + tangentPoints[i].z/3.00000;

    return p2;

}

/* Function to evaluate bezier point from second tangent */
struct point point3(struct point points[100],int i,int n,struct point tangentPoints[100])
{
    struct point p3;

    p3.x = (points[i].x - tangentPoints[i].x/3.00000);
    p3.y = (points[i].y - tangentPoints[i].y/3.00000);
    p3.z = (points[i].z - tangentPoints[i].z/3.00000);

    return p3;
}

int main(int argc, char** argv)
{
    string filename;
    filename = "cpts_in.txt";
    double du = 0.09,radius = 0.1;
    double T = 0.0;
    // loop to take arguments from command line else take up default values
    for (int i = 0; i < argc; ++i)
    {
        if( strcmp(argv[i],"-f") == 0)
        {
           i++;
           filename = argv[i];
        }
        else if(strcmp(argv[i], "-u") == 0)
        {
           i++;
           du = atof(argv[i]);
           //cout<<argv[i]<<" ";
        }
        else if(strcmp(argv[i], "-r") == 0)
        {
           i++;
           radius = atof(argv[i]);
           //cout<<argv[i]<<" ";
        }
        else if(strcmp(argv[i], "-t") == 0)
        {
           i++;
           T = atof(argv[i]);
           //cout<<argv[i]<<" ";
        }
    }

    ifstream file;
    string line;
    file.open(filename.c_str());
    struct point points[100];
    struct point givenTangents[2];

    int n = 0;
    int i =0;
    // loop to read from the input file
    for(;;)
    {
        getline(file,line);
		if(file.eof()) {
            break;
        }
        if(i<=1)
        {
            struct point p;

            std::stringstream linestream(line);

            double value;

            linestream >> value;

            p.x = value;

            linestream >> value;

            p.y = value;

            linestream >> value;

            p.z = value;

            givenTangents[i] = p;
        }
        else
        {
            struct point p;

            std::stringstream linestream(line);

            double value;

            linestream >> value;


            p.x = value;

            linestream >> value;

            p.y = value;

            linestream >> value;

            p.z = value;

            points[n] = p;

            n++;
        }
        i++;
    }
    struct point tangentPoints[100];

    // Call the tangent function
    for(int i = 1;i<n;i++)
    {
        if(i!=0 && i!=n-1)
        {
            tangentPoints[i] = tangentPoint(i,points,n,T);
        }
    }

    // Multiply the first and last tangent with tension
    tangentPoints[0].x = ((1-T)*givenTangents[0].x);
    tangentPoints[0].y = ((1-T)*givenTangents[0].y);
    tangentPoints[0].z = ((1-T)*givenTangents[0].z);
    tangentPoints[n-1].x = ((1-T)*givenTangents[1].x);
    tangentPoints[n-1].y = ((1-T)*givenTangents[1].y);
    tangentPoints[n-1].z = ((1-T)*givenTangents[1].z);

    struct point updatedPoints[100];

    int updated = 0;

    // Generate bezier points from tangents evaluated
    for(int i = 0;i < n-1;i++)
    {
        updatedPoints[updated++] = points[i];
        updatedPoints[updated++] = point2(points,i,n,tangentPoints);
        i++;
        updatedPoints[updated++] = point3(points,i,n,tangentPoints);
        i--;
    }

    updatedPoints[updated] = points[n-1];

    cout<<"#Inventor V2.0 ascii \n\nSeparator {LightModel {model BASE_COLOR} Material {diffuseColor 1.0 1.0 1.0} \n";
    cout<<"Coordinate3 { 	point [\n";

    int bezpoint = 0;

    // Loop to calculate bezier points and concatenate to form Catmull-Rom spline 
    for(int i=0; i<updated; i=i+3)
    {
        struct point bez[4];
        struct point s = {0,0,0};
        bez[0] = updatedPoints[i];
        bez[1] = updatedPoints[i+1];
        bez[2] = updatedPoints[i+2];
        bez[3] = updatedPoints[i+3];

        for(double t = 0.0 ;t <= 1.0;t += du)
        {
            struct point p = bezierEquation(4,t,bez);
            bezpoint++;
            cout<<p.x << " "<<p.y<<" "<<p.z <<"\n";
        }
    }

    cout<< points[n-1].x<<" "<<points[n-1].y<<" "<<points[n-1].z<<"\n";
    cout<< "] }\n";
    cout<< "IndexedLineSet {coordIndex [\n";
    int q=0;

    for(int q=0; q<=bezpoint; q++)
    {
       cout<< q <<", ";
    }

    cout<<"-1,\n";
    cout<<"] } }\n";

    for(int i=0;i<n;i++)
    {
        cout<<"Separator {LightModel {model PHONG}Material {	diffuseColor 1.0 1.0 1.0}\n";
        cout<<"Transform {translation\n";
        cout<<points[i].x<<" "<<points[i].y<<" "<<points[i].z;
        cout<<"}Sphere {	          radius " <<radius<<" }}";
    }
    return 0;
}