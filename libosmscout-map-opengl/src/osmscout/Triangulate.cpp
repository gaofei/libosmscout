/*
  This source is part of the libosmscout-map library
  Copyright (C) 2017  Fanny Monori

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/
//#include <utility>
#include <osmscout/Triangulate.h>

#include <osmscout/system/Math.h>

#include <poly2tri/poly2tri.h>

namespace osmscout {
  std::vector<GLfloat> osmscout::Triangulate::TriangulatePolygon(std::vector<osmscout::Point> points) {
    std::vector<GLfloat> result;

    std::vector<p2t::Point *> polyline;
    std::for_each(points.begin(), points.end(),
                  [&polyline](osmscout::Point p) { polyline.push_back(new p2t::Point(p.GetLon(), p.GetLat())); });
    p2t::CDT *cdt = new p2t::CDT(polyline);
    cdt->Triangulate();
    std::vector<p2t::Triangle *> triangles;
    triangles = cdt->GetTriangles();
    for (int i = 0; i < triangles.size(); i++) {
      p2t::Point &a = *triangles[i]->GetPoint(0);
      p2t::Point &b = *triangles[i]->GetPoint(1);
      p2t::Point &c = *triangles[i]->GetPoint(2);
      result.emplace_back(a.x);
      result.emplace_back(a.y);
      result.emplace_back(b.x);
      result.emplace_back(b.y);
      result.emplace_back(c.x);
      result.emplace_back(c.y);
    }

    return result;

  }

  std::vector<std::vector<osmscout::Point>> osmscout::Triangulate::TriangulateP(std::vector<osmscout::Point> points){
    std::vector<std::vector<osmscout::Point>> result;
    std::vector<p2t::Point *> polyline;
    std::for_each(points.begin(), points.end(),
                  [&polyline](osmscout::Point p) { polyline.push_back(new p2t::Point(p.GetLon(), p.GetLat())); });
    p2t::CDT *cdt = new p2t::CDT(polyline);
    cdt->Triangulate();
    std::vector<p2t::Triangle *> triangles;
    triangles = cdt->GetTriangles();
    for (int i = 0; i < triangles.size(); i++) {
      p2t::Point &a = *triangles[i]->GetPoint(0);
      p2t::Point &b = *triangles[i]->GetPoint(1);
      p2t::Point &c = *triangles[i]->GetPoint(2);
      std::vector<osmscout::Point> ps;
      osmscout::Point ap;
      ap.SetCoord(osmscout::GeoCoord(a.y,a.x));
      osmscout::Point bp;
      bp.SetCoord(osmscout::GeoCoord(b.y,b.x));
      osmscout::Point cp;
      cp.SetCoord(osmscout::GeoCoord(c.y,c.x));
      ps.push_back(ap);
      ps.push_back(bp);
      ps.push_back(cp);
      result.push_back(ps);
    }

    return result;
  }

  std::vector<GLfloat> osmscout::Triangulate::TriangulatePolygon(std::vector<osmscout::Vertex2D> points) {
    std::vector<GLfloat> result;

    std::vector<p2t::Point *> polyline;
    std::for_each(points.begin(), points.end(),
                  [&polyline](osmscout::Vertex2D p) { polyline.push_back(new p2t::Point(p.GetX(), p.GetY())); });
    p2t::CDT *cdt = new p2t::CDT(polyline);
    cdt->Triangulate();
    std::vector<p2t::Triangle *> triangles;
    triangles = cdt->GetTriangles();
    for (int i = 0; i < triangles.size(); i++) {
      p2t::Point &a = *triangles[i]->GetPoint(0);
      p2t::Point &b = *triangles[i]->GetPoint(1);
      p2t::Point &c = *triangles[i]->GetPoint(2);
      result.emplace_back(a.x);
      result.emplace_back(a.y);
      result.emplace_back(b.x);
      result.emplace_back(b.y);
      result.emplace_back(c.x);
      result.emplace_back(c.y);
    }

    return result;

  }

  std::vector<GLfloat> osmscout::Triangulate::TriangulatePolygon(std::vector<osmscout::GeoCoord> points) {
    std::vector<GLfloat> result;

    std::vector<p2t::Point *> polyline;
    std::for_each(points.begin(), points.end(),
                  [&polyline](osmscout::GeoCoord g) { polyline.push_back(new p2t::Point(g.GetLon(), g.GetLat())); });
    p2t::CDT *cdt = new p2t::CDT(polyline);
    cdt->Triangulate();
    std::vector<p2t::Triangle *> triangles;
    triangles = cdt->GetTriangles();
    for (int i = 0; i < triangles.size(); i++) {
      p2t::Point &a = *triangles[i]->GetPoint(0);
      p2t::Point &b = *triangles[i]->GetPoint(1);
      p2t::Point &c = *triangles[i]->GetPoint(2);
      result.emplace_back(a.x);
      result.emplace_back(a.y);
      result.emplace_back(b.x);
      result.emplace_back(b.y);
      result.emplace_back(c.x);
      result.emplace_back(c.y);
    }

    return result;

  }

  void osmscout::Triangulate::TriangulatePolygon(std::vector<osmscout::Point> points, std::vector<osmscout::Point>  &result) {
    std::vector<p2t::Point *> polyline;
    std::for_each(points.begin(), points.end(),
                  [&polyline](osmscout::Point g) { polyline.push_back(new p2t::Point(g.GetLon(), g.GetLat())); });
    p2t::CDT *cdt = new p2t::CDT(polyline);
    cdt->Triangulate();
    std::vector<p2t::Triangle *> triangles;
    triangles = cdt->GetTriangles();
    for (int i = 0; i < triangles.size(); i++) {
      p2t::Point &a = *triangles[i]->GetPoint(0);
      p2t::Point &b = *triangles[i]->GetPoint(1);
      p2t::Point &c = *triangles[i]->GetPoint(2);
      osmscout::Point ap;
      ap.SetCoord(osmscout::GeoCoord(a.y,a.x));
      osmscout::Point bp;
      bp.SetCoord(osmscout::GeoCoord(b.y,b.x));
      osmscout::Point cp;
      cp.SetCoord(osmscout::GeoCoord(c.y,c.x));
      result.emplace_back(ap);
      result.emplace_back(bp);
      result.emplace_back(cp);
    }
  }

  std::vector<GLfloat> osmscout::Triangulate::TriangulateWithHoles(std::vector<std::vector<osmscout::Point>> points) {
    std::vector<GLfloat> result;
    std::vector<p2t::Point *> polyline;
    std::for_each(points[0].begin(), points[0].end(),
                  [&polyline](osmscout::Point p) { polyline.push_back(new p2t::Point(p.GetLon(), p.GetLat())); });
    p2t::CDT *cdt = new p2t::CDT(polyline);

    for (int i = 1; i < points.size(); i++) {
      std::vector<p2t::Point *> hole;
      std::for_each(points[i].begin(), points[i].end(),
                    [&hole](osmscout::Point p) { hole.push_back(new p2t::Point(p.GetLon(), p.GetLat())); });
      cdt->AddHole(hole);
    }

    cdt->Triangulate();
    std::vector<p2t::Triangle *> triangles;
    triangles = cdt->GetTriangles();
    for (int i = 0; i < triangles.size(); i++) {
      p2t::Point &a = *triangles[i]->GetPoint(0);
      p2t::Point &b = *triangles[i]->GetPoint(1);
      p2t::Point &c = *triangles[i]->GetPoint(2);
      result.emplace_back(a.x);
      result.emplace_back(a.y);
      result.emplace_back(b.x);
      result.emplace_back(b.y);
      result.emplace_back(c.x);
      result.emplace_back(c.y);
    }

    return result;

  }

}
