#ifndef BETASERIESAPI_GLOBAL_H
#define BETASERIESAPI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BETASERIESAPI_H)
#  define BETASERIESAPISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BETASERIESAPISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // BetaSeriesApi_GLOBAL_H
