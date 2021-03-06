/***************************************************************************
    qgsmaptoolrectanglecenter.cpp  -  map tool for adding rectangle
    from center and a point
    ---------------------
    begin                : July 2017
    copyright            : (C) 2017 by Loïc Bartoletti
    email                : lbartoletti at tuxfamily dot org
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsmaptoolrectanglecenter.h"
#include "qgsgeometryrubberband.h"
#include "qgsgeometryutils.h"
#include "qgsmapcanvas.h"
#include "qgslinestring.h"
#include "qgspoint.h"
#include "qgsmapmouseevent.h"
#include "qgssnapindicator.h"

#include <memory>

QgsMapToolRectangleCenter::QgsMapToolRectangleCenter( QgsMapToolCapture *parentTool,
    QgsMapCanvas *canvas, CaptureMode mode )
  : QgsMapToolAddRectangle( parentTool, canvas, mode )
{
}

void QgsMapToolRectangleCenter::cadCanvasReleaseEvent( QgsMapMouseEvent *e )
{
  QgsPoint point = mapPoint( *e );

  if ( e->button() == Qt::LeftButton )
  {
    mPoints.append( point );

    if ( !mPoints.isEmpty() && !mTempRubberBand )
    {
      mTempRubberBand = createGeometryRubberBand( mLayerType, true );
      mTempRubberBand->show();
    }
  }
  else if ( e->button() == Qt::RightButton )
  {
    mPoints.append( point );

    deactivate();
    if ( mParentTool )
    {
      mParentTool->canvasReleaseEvent( e );
    }
  }
}

void QgsMapToolRectangleCenter::cadCanvasMoveEvent( QgsMapMouseEvent *e )
{
  QgsPoint point = mapPoint( *e );

  mSnapIndicator->setMatch( e->mapPointMatch() );

  if ( mTempRubberBand )
  {
    switch ( mPoints.size() )
    {
      case 1:
      {
        if ( qgsDoubleNear( mCanvas->rotation(), 0.0 ) )
        {
          double xOffset = fabs( point.x() - mPoints.at( 0 ).x() );
          double yOffset = fabs( point.y() - mPoints.at( 0 ).y() );

          mRectangle = QgsBox3d( QgsPoint( mPoints.at( 0 ).x() - xOffset, mPoints.at( 0 ).y() - yOffset, mPoints.at( 0 ).z() ), QgsPoint( mPoints.at( 0 ).x() + xOffset, mPoints.at( 0 ).y() + yOffset, mPoints.at( 0 ).z() ) );

          mTempRubberBand->setGeometry( QgsMapToolAddRectangle::rectangleToPolygon() );
        }
        else
        {
          double dist = mPoints.at( 0 ).distance( point );
          double angle = mPoints.at( 0 ).azimuth( point );

          mRectangle = QgsBox3d( mPoints.at( 0 ).project( -dist, angle ), mPoints.at( 0 ).project( dist, angle ) );
          mTempRubberBand->setGeometry( QgsMapToolAddRectangle::rectangleToPolygon() );
        }
      }
      break;
      default:
        break;
    }
  }
}
