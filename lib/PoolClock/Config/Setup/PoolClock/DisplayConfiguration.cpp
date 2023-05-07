/**
 * \file DisplayConfiguration.cpp
 * \author Florian laschober
 * \brief Configuration for the whole LED setup.
 *
 */

//This configuration is for a fully loaded 24h display with intermediate segments.

#include "DisplayManager.h"

/**
 * \addtogroup DisplayConfiguration
 * \brief Configuration to tell the system how the LEDs are wired together and arranged.
 *  \{
 */

/**
 * \brief Each segment belongs to some display. This array defines the segment position within this one display.
 *		  The order of these has to mach the order in which the LEDs are wired.
 */
SevenSegment::SegmentPosition DisplayManager::SegmentPositions[NUM_SEGMENTS] = {
	SevenSegment::LeftTopSegment,
	SevenSegment::MiddleTopSegment,
	SevenSegment::RightTopSegment,
	SevenSegment::CenterSegment,
	SevenSegment::LeftBottomSegment,
	SevenSegment::MiddleBottomSegment,
	SevenSegment::RightBottomSegment,

	SevenSegment::LeftTopSegment,
	SevenSegment::MiddleTopSegment,
	SevenSegment::RightTopSegment,
	SevenSegment::CenterSegment,
	SevenSegment::LeftBottomSegment,
	SevenSegment::MiddleBottomSegment,
	SevenSegment::RightBottomSegment,

	SevenSegment::LeftTopSegment,
	SevenSegment::MiddleTopSegment,
	SevenSegment::RightTopSegment,
	SevenSegment::CenterSegment,
	SevenSegment::LeftBottomSegment,
	SevenSegment::MiddleBottomSegment,
	SevenSegment::RightBottomSegment,

	SevenSegment::LeftTopSegment,
	SevenSegment::MiddleTopSegment,
	SevenSegment::RightTopSegment,
	SevenSegment::CenterSegment,
	SevenSegment::LeftBottomSegment,
	SevenSegment::MiddleBottomSegment,
	SevenSegment::RightBottomSegment,

	SevenSegment::LeftTopSegment,
	SevenSegment::LeftBottomSegment,

	SevenSegment::LeftTopSegment,
	SevenSegment::MiddleTopSegment,
	SevenSegment::RightTopSegment,
	SevenSegment::CenterSegment,
	SevenSegment::LeftBottomSegment,
	SevenSegment::MiddleBottomSegment,
	SevenSegment::RightBottomSegment,

	SevenSegment::LeftTopSegment,
	SevenSegment::MiddleTopSegment,
	SevenSegment::RightTopSegment,
	SevenSegment::CenterSegment,
	SevenSegment::LeftBottomSegment,
	SevenSegment::MiddleBottomSegment,
	SevenSegment::RightBottomSegment,

	SevenSegment::LeftTopSegment,
	SevenSegment::MiddleTopSegment,
	SevenSegment::RightTopSegment,
	SevenSegment::CenterSegment,
	SevenSegment::LeftBottomSegment,
	SevenSegment::MiddleBottomSegment,
	SevenSegment::RightBottomSegment,

	SevenSegment::LeftTopSegment,
	SevenSegment::MiddleTopSegment,
	SevenSegment::RightTopSegment,
	SevenSegment::CenterSegment,
	SevenSegment::LeftBottomSegment,
	SevenSegment::MiddleBottomSegment,
	SevenSegment::RightBottomSegment
};

/**
 * \brief Each segment has a direction, this is important for animation.
 * 		  The order of them is the same as #DisplayManager::SegmentPositions and the direction has to match the
 *        sequence in which the LEDs are wired.
 */
Segment::direction DisplayManager::SegmentDirections[NUM_SEGMENTS] = {
	Segment::BOTTOM_TO_TOP,
	Segment::LEFT_TO_RIGHT,
	Segment::TOP_TO_BOTTTOM,
	Segment::RIGHT_TO_LEFT,
	Segment::TOP_TO_BOTTTOM,
	Segment::LEFT_TO_RIGHT,
	Segment::BOTTOM_TO_TOP,

	Segment::BOTTOM_TO_TOP,
	Segment::LEFT_TO_RIGHT,
	Segment::TOP_TO_BOTTTOM,
	Segment::RIGHT_TO_LEFT,
	Segment::TOP_TO_BOTTTOM,
	Segment::LEFT_TO_RIGHT,
	Segment::BOTTOM_TO_TOP,

	Segment::BOTTOM_TO_TOP,
	Segment::LEFT_TO_RIGHT,
	Segment::TOP_TO_BOTTTOM,
	Segment::RIGHT_TO_LEFT,
	Segment::TOP_TO_BOTTTOM,
	Segment::LEFT_TO_RIGHT,
	Segment::BOTTOM_TO_TOP,

	Segment::BOTTOM_TO_TOP,
	Segment::LEFT_TO_RIGHT,
	Segment::TOP_TO_BOTTTOM,
	Segment::RIGHT_TO_LEFT,
	Segment::TOP_TO_BOTTTOM,
	Segment::LEFT_TO_RIGHT,
	Segment::BOTTOM_TO_TOP,

	Segment::TOP_TO_BOTTTOM,
	Segment::TOP_TO_BOTTTOM,

	Segment::BOTTOM_TO_TOP,
	Segment::LEFT_TO_RIGHT,
	Segment::TOP_TO_BOTTTOM,
	Segment::RIGHT_TO_LEFT,
	Segment::TOP_TO_BOTTTOM,
	Segment::LEFT_TO_RIGHT,
	Segment::BOTTOM_TO_TOP,

	Segment::BOTTOM_TO_TOP,
	Segment::LEFT_TO_RIGHT,
	Segment::TOP_TO_BOTTTOM,
	Segment::RIGHT_TO_LEFT,
	Segment::TOP_TO_BOTTTOM,
	Segment::LEFT_TO_RIGHT,
	Segment::BOTTOM_TO_TOP,

	Segment::BOTTOM_TO_TOP,
	Segment::LEFT_TO_RIGHT,
	Segment::TOP_TO_BOTTTOM,
	Segment::RIGHT_TO_LEFT,
	Segment::TOP_TO_BOTTTOM,
	Segment::LEFT_TO_RIGHT,
	Segment::BOTTOM_TO_TOP,

	Segment::BOTTOM_TO_TOP,
	Segment::LEFT_TO_RIGHT,
	Segment::TOP_TO_BOTTTOM,
	Segment::RIGHT_TO_LEFT,
	Segment::TOP_TO_BOTTTOM,
	Segment::LEFT_TO_RIGHT,
	Segment::BOTTOM_TO_TOP
};

/**
 * \brief Displays that are present. These define the displays in the order that is set in the #DisplayManager::diplayIndex array.
 */
SevenSegment::SevenSegmentMode DisplayManager::SegmentDisplayModes[NUM_DISPLAYS] = {
	SevenSegment::FULL_SEGMENT,
	SevenSegment::FULL_SEGMENT,
	SevenSegment::FULL_SEGMENT,
	SevenSegment::FULL_SEGMENT,
	SevenSegment::ONLY_ONE,
	SevenSegment::FULL_SEGMENT,
	SevenSegment::FULL_SEGMENT,
	SevenSegment::FULL_SEGMENT,
	SevenSegment::FULL_SEGMENT
};

/**
 * \brief Displays that are present. These define the displays in the order that is set in the #DisplayManager::diplayIndex array.
 */
SevenSegment::SevenSegmentSize DisplayManager::SegmentDisplaySize[NUM_DISPLAYS] = {
	SevenSegment::SHORT_SEGMENT,
	SevenSegment::SHORT_SEGMENT,
	SevenSegment::LONG_SEGMENT,
	SevenSegment::LONG_SEGMENT,
	SevenSegment::DOT_SEGMENT,
	SevenSegment::LONG_SEGMENT,
	SevenSegment::LONG_SEGMENT,
	SevenSegment::SHORT_SEGMENT,
	SevenSegment::SHORT_SEGMENT

};

/**
 * \brief These indicies correspond to the index of a Display in the array above (#DisplayManager::SegmentDisplayModes).
 * 		  They define which segment belongs to which Display in the order that they are wired in.
 *        The enum #DisplayIDs from \ref Configuration.h can also be used to create a more readable config.
 */
uint8_t DisplayManager::diplayIndex[NUM_SEGMENTS] = {
	HIGHER_DIGIT_TEMP1_DISPLAY,
	HIGHER_DIGIT_TEMP1_DISPLAY,
	HIGHER_DIGIT_TEMP1_DISPLAY,
	HIGHER_DIGIT_TEMP1_DISPLAY,
	HIGHER_DIGIT_TEMP1_DISPLAY,
	HIGHER_DIGIT_TEMP1_DISPLAY,
	HIGHER_DIGIT_TEMP1_DISPLAY,

	LOWER_DIGIT_TEMP1_DISPLAY,
	LOWER_DIGIT_TEMP1_DISPLAY,
	LOWER_DIGIT_TEMP1_DISPLAY,
	LOWER_DIGIT_TEMP1_DISPLAY,
	LOWER_DIGIT_TEMP1_DISPLAY,
	LOWER_DIGIT_TEMP1_DISPLAY,
	LOWER_DIGIT_TEMP1_DISPLAY,

	HIGHER_DIGIT_HOUR_DISPLAY,
	HIGHER_DIGIT_HOUR_DISPLAY,
	HIGHER_DIGIT_HOUR_DISPLAY,
	HIGHER_DIGIT_HOUR_DISPLAY,
	HIGHER_DIGIT_HOUR_DISPLAY,
	HIGHER_DIGIT_HOUR_DISPLAY,
	HIGHER_DIGIT_HOUR_DISPLAY,

	LOWER_DIGIT_HOUR_DISPLAY,
	LOWER_DIGIT_HOUR_DISPLAY,
	LOWER_DIGIT_HOUR_DISPLAY,
	LOWER_DIGIT_HOUR_DISPLAY,
	LOWER_DIGIT_HOUR_DISPLAY,
	LOWER_DIGIT_HOUR_DISPLAY,
	LOWER_DIGIT_HOUR_DISPLAY,

	HIGHER_DIGIT_MINUTE_DISPLAY,
	HIGHER_DIGIT_MINUTE_DISPLAY,
	HIGHER_DIGIT_MINUTE_DISPLAY,
	HIGHER_DIGIT_MINUTE_DISPLAY,
	HIGHER_DIGIT_MINUTE_DISPLAY,
	HIGHER_DIGIT_MINUTE_DISPLAY,
	HIGHER_DIGIT_MINUTE_DISPLAY,

	LOWER_DIGIT_MINUTE_DISPLAY,
	LOWER_DIGIT_MINUTE_DISPLAY,
	LOWER_DIGIT_MINUTE_DISPLAY,
	LOWER_DIGIT_MINUTE_DISPLAY,
	LOWER_DIGIT_MINUTE_DISPLAY,
	LOWER_DIGIT_MINUTE_DISPLAY,
	LOWER_DIGIT_MINUTE_DISPLAY,

	HIGHER_DIGIT_TEMP2_DISPLAY,
	HIGHER_DIGIT_TEMP2_DISPLAY,
	HIGHER_DIGIT_TEMP2_DISPLAY,
	HIGHER_DIGIT_TEMP2_DISPLAY,
	HIGHER_DIGIT_TEMP2_DISPLAY,
	HIGHER_DIGIT_TEMP2_DISPLAY,
	HIGHER_DIGIT_TEMP2_DISPLAY,

	LOWER_DIGIT_TEMP2_DISPLAY,
	LOWER_DIGIT_TEMP2_DISPLAY,
	LOWER_DIGIT_TEMP2_DISPLAY,
	LOWER_DIGIT_TEMP2_DISPLAY,
	LOWER_DIGIT_TEMP2_DISPLAY,
	LOWER_DIGIT_TEMP2_DISPLAY,
	LOWER_DIGIT_TEMP2_DISPLAY
};

/** \}*/
