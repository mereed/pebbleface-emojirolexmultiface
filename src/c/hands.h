#pragma once

#include "pebble.h"

#if PBL_PLATFORM_CHALK	
static const GPathInfo MINUTE_HAND_POINTS = {
	4,
  (GPoint []) {
    { -5, 0 },
    { 0, -80 },
    { 5, 0 },
	{ -5, 0 },
	
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
4,
  (GPoint []) {
    { -4, 0 },
    { 0,-45 },
	{ 4, 0 },
	{ -4, 0 },		
  }
};

static const GPathInfo SECOND_HAND_POINTS = {
  4,
  (GPoint []) {
    { -3, 30 },
    { 0, -90 },
    { 3, 30 },
	{ -3, 30 },
  }
};

#else
static const GPathInfo MINUTE_HAND_POINTS = {
	4,
  (GPoint []) {
    { -5, 0 },
    { 0, -80 },
    { 5, 0 },
	{ -5, 0 },
	
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
4,
  (GPoint []) {
    { -4, 0 },
    { 0,-45 },
	{ 4, 0 },
	{ -4, 0 },		
  }
};

static const GPathInfo SECOND_HAND_POINTS = {
  4,
  (GPoint []) {
    { -3, 30 },
    { 0, -90 },
    { 3, 30 },
	{ -3, 30 },
  }
};
#endif