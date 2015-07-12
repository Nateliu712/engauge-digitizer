#include "Curve.h"
#include "CurveStyles.h"
#include "DataKey.h"
#include "EngaugeAssert.h"
#include "GraphicsLinesForCurve.h"
#include "GraphicsLinesForCurves.h"
#include "GraphicsPoint.h"
#include "GraphicsPointAbstractBase.h"
#include "GraphicsScene.h"
#include <iostream>
#include "Logger.h"
#include "Point.h"
#include <QGraphicsItem>
#include <QTextStream>
#include "QtToString.h"
#include "Transformation.h"

GraphicsLinesForCurves::GraphicsLinesForCurves()
{
}

void GraphicsLinesForCurves::addPoint (const QString &curveName,
                                       const QString &pointIdentifier,
                                       GraphicsPoint &point)
{
  LOG4CPP_INFO_S ((*mainCat)) << "GraphicsLinesForCurves::addPoint"
                              << " curve=" << curveName.toLatin1().data()
                              << " identifier=" << pointIdentifier.toLatin1().data()
                              << " ordinal=" << point.data (DATA_KEY_ORDINAL).toInt()
                              << " pos=" << QPointFToString (point.pos()).toLatin1().data();

  m_graphicsLinesForCurve [curveName]->addPoint (pointIdentifier,
                                                 point);
}

void GraphicsLinesForCurves::addRemoveCurves (GraphicsScene &scene,
                                              const QStringList &curveNames)
{
  // Add new curves
  QStringList::const_iterator itrC;
  for (itrC = curveNames.begin (); itrC != curveNames.end (); itrC++) {

    QString curveName = *itrC;

    if (!m_graphicsLinesForCurve.contains (curveName)) {

      GraphicsLinesForCurve *item = new GraphicsLinesForCurve(curveName);
      scene.addItem (item);

      m_graphicsLinesForCurve [curveName] = item;
    }
  }

  // Remove expired curves
  GraphicsLinesContainer::const_iterator itrG, itrGNext;
  for (itrG = m_graphicsLinesForCurve.begin (); itrG != m_graphicsLinesForCurve.end (); itrG = itrGNext) {

    const QString curveName = itrG.key ();
    GraphicsLinesForCurve *graphicsLines = itrG.value();

    itrGNext = itrG;
    itrGNext++;

    if (!curveNames.contains (curveName)) {

      delete graphicsLines;
      m_graphicsLinesForCurve.remove (curveName);
    }
  }
}

void GraphicsLinesForCurves::lineMembershipPurge(const CurveStyles &curveStyles)
{
  LOG4CPP_INFO_S ((*mainCat)) << "GraphicsLinesForCurves::lineMembershipPurge";

  GraphicsLinesContainer::const_iterator itr;
  for (itr = m_graphicsLinesForCurve.begin (); itr != m_graphicsLinesForCurve.end (); itr++) {

    const QString curveName = itr.key ();
    GraphicsLinesForCurve *graphicsLines = itr.value();

    graphicsLines->lineMembershipPurge (curveStyles.lineStyle (curveName));
  }
}

void GraphicsLinesForCurves::lineMembershipReset()
{
  LOG4CPP_INFO_S ((*mainCat)) << "GraphicsLinesForCurves::lineMembershipReset";

  GraphicsLinesContainer::const_iterator itr;
  for (itr = m_graphicsLinesForCurve.begin (); itr != m_graphicsLinesForCurve.end (); itr++) {

    GraphicsLinesForCurve *graphicsLines = itr.value();

    graphicsLines->lineMembershipReset ();
  }
}

void GraphicsLinesForCurves::print () const
{
  QString text;
  QTextStream str (&text);

  printStream ("", str);
  std::cerr << text.toLatin1().data();
}

void GraphicsLinesForCurves::printStream (QString indentation,
                                          QTextStream &str) const
{
  str << indentation << "GraphicsLinesForCurves\n";

  indentation += INDENTATION_DELTA;

  GraphicsLinesContainer::const_iterator itr;
  for (itr = m_graphicsLinesForCurve.begin (); itr != m_graphicsLinesForCurve.end (); itr++) {

    const GraphicsLinesForCurve *graphicsLines = itr.value();

    graphicsLines->printStream (indentation,
                                str);
  }
}

void GraphicsLinesForCurves::removePoint(const QString &identifier)
{
  LOG4CPP_INFO_S ((*mainCat)) << "GraphicsLinesForCurves::removePoint"
                              << " point=" << identifier.toLatin1().data ();

  QString curveName = Point::curveNameFromPointIdentifier(identifier);

  ENGAUGE_ASSERT (m_graphicsLinesForCurve.contains (curveName));
  double ordinal = m_graphicsLinesForCurve [curveName]->identifierToOrdinal (identifier);
  m_graphicsLinesForCurve [curveName]->removePoint(ordinal);
}

void GraphicsLinesForCurves::updateAfterCommand (GraphicsScene &scene,
                                                 const CurveStyles &curveStyles,
                                                 const QString &curveName,
                                                 const Point &point)
{
  LOG4CPP_INFO_S ((*mainCat)) << "GraphicsLinesForCurves::updateAfterCommand"
                              << " point=" << point.identifier().toLatin1().data();

  ENGAUGE_ASSERT (m_graphicsLinesForCurve.contains (curveName));
  m_graphicsLinesForCurve [curveName]->updateAfterCommand (scene,
                                                           curveStyles.pointStyle(curveName),
                                                           point);
}

void GraphicsLinesForCurves::updateGraphicsLinesToMatchGraphicsPoints (const CurveStyles &curveStyles)
{
  LOG4CPP_INFO_S ((*mainCat)) << "GraphicsLinesForCurves::updateGraphicsLinesToMatchGraphicsPoints";

  GraphicsLinesContainer::const_iterator itr;
  for (itr = m_graphicsLinesForCurve.begin (); itr != m_graphicsLinesForCurve.end (); itr++) {

    QString curveName = itr.key();

    // This is where we add lines for non-axes curves
    if (curveName != AXIS_CURVE_NAME) {

      m_graphicsLinesForCurve [curveName]->updateGraphicsLinesToMatchGraphicsPoints(curveStyles.lineStyle (curveName));
    }
  }
}

void GraphicsLinesForCurves::updatePointOrdinalsAfterDrag (const CurveStyles &curveStyles,
                                                           const Transformation &transformation)
{
  LOG4CPP_INFO_S ((*mainCat)) << "GraphicsLinesForCurves::updatePointOrdinalsAfterDrag";

  GraphicsLinesContainer::const_iterator itr;
  for (itr = m_graphicsLinesForCurve.begin (); itr != m_graphicsLinesForCurve.end (); itr++) {

    QString curveName = itr.key();
    GraphicsLinesForCurve *graphicsLines = itr.value();

    graphicsLines->updatePointOrdinalsAfterDrag (curveStyles.lineStyle (curveName),
                                                 transformation);
  }
}
