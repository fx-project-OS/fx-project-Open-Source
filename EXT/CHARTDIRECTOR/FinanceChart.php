<?php
require_once("../lib/phpchartdir.php");

#/////////////////////////////////////////////////////////////////////////////////////////////////
# Copyright 2012 Advanced Software Engineering Limited
#
# ChartDirector FinanceChart class library
#     - Requires ChartDirector Ver 5.1 or above
#
# You may use and modify the code in this file in your application, provided the code and
# its modifications are used only in conjunction with ChartDirector. Usage of this software
# is subjected to the terms and condition of the ChartDirector license.
#/////////////////////////////////////////////////////////////////////////////////////////////////

#/ <summary>
#/ Represents a Financial Chart
#/ </summary>
class FinanceChart extends MultiChart
{
    var $m_totalWidth = 0;
    var $m_totalHeight = 0;
    var $m_antiAlias = true;
    var $m_logScale = false;
    var $m_axisOnRight = true;

    var $m_leftMargin = 40;
    var $m_rightMargin = 40;
    var $m_topMargin = 30;
    var $m_bottomMargin = 35;

    var $m_plotAreaBgColor = 0xffffff;
    var $m_plotAreaBorder = 0x888888;
    var $m_plotAreaGap = 2;

    var $m_majorHGridColor = 0xdddddd;
    var $m_minorHGridColor = 0xdddddd;
    var $m_majorVGridColor = 0xdddddd;
    var $m_minorVGridColor = 0xdddddd;

    var $m_legendFont = "normal";
    var $m_legendFontSize = 8;
    var $m_legendFontColor = TextColor;
    var $m_legendBgColor = 0x80cccccc;

    var $m_yAxisFont = "normal";
    var $m_yAxisFontSize = 8;
    var $m_yAxisFontColor = TextColor;
    var $m_yAxisMargin = 14;

    var $m_xAxisFont = "normal";
    var $m_xAxisFontSize = 8;
    var $m_xAxisFontColor = TextColor;
    var $m_xAxisFontAngle = 0;

    var $m_timeStamps = null;
    var $m_highData = null;
    var $m_lowData = null;
    var $m_openData = null;
    var $m_closeData = null;
    var $m_volData = null;
    var $m_volUnit = "";
    var $m_extraPoints = 0;

    var $m_yearFormat = "{value|yyyy}";
    var $m_firstMonthFormat = "<*font=bold*>{value|mmm yy}";
    var $m_otherMonthFormat = "{value|mmm}";
    var $m_firstDayFormat = "<*font=bold*>{value|d mmm}";
    var $m_otherDayFormat = "{value|d}";
    var $m_firstHourFormat = "<*font=bold*>{value|d mmm\nh:nna}";
    var $m_otherHourFormat = "{value|h:nna}";
    var $m_timeLabelSpacing = 50;

    var $m_generalFormat = "P3";

    var $m_toolTipMonthFormat = "[{xLabel|mmm yyyy}]";
    var $m_toolTipDayFormat = "[{xLabel|mmm d, yyyy}]";
    var $m_toolTipHourFormat = "[{xLabel|mmm d, yyyy hh:nn:ss}]";

    var $m_mainChart = null;
    var $m_currentChart = null;

    #/ <summary>
    #/ Create a FinanceChart with a given width. The height will be automatically determined
    #/ as the chart is built.
    #/ </summary>
    #/ <param name="width">Width of the chart in pixels</param>
    function __construct($width) {
        parent::__construct($width, 1);
        $this->m_totalWidth = $width;
        $this->setMainChart($this);
    }

    #/ <summary>
    #/ Enable/Disable anti-alias. Enabling anti-alias makes the line smoother. Disabling
    #/ anti-alias make the chart file size smaller, and so can be downloaded faster
    #/ through the Internet. The default is to enable anti-alias.
    #/ </summary>
    #/ <param name="antiAlias">True to enable anti-alias. False to disable anti-alias.</param>
    function enableAntiAlias($antiAlias) {
        $this->m_antiAlias = $antiAlias;
    }

    #/ <summary>
    #/ Set the margins around the plot area.
    #/ </summary>
    #/ <param name="m_leftMargin">The distance between the plot area and the chart left edge.</param>
    #/ <param name="m_topMargin">The distance between the plot area and the chart top edge.</param>
    #/ <param name="m_rightMargin">The distance between the plot area and the chart right edge.</param>
    #/ <param name="m_bottomMargin">The distance between the plot area and the chart bottom edge.</param>
    function setMargins($leftMargin, $topMargin, $rightMargin, $bottomMargin) {
        $this->m_leftMargin = $leftMargin;
        $this->m_rightMargin = $rightMargin;
        $this->m_topMargin = $topMargin;
        $this->m_bottomMargin = $bottomMargin;
    }

    #/ <summary>
    #/ Add a text title above the plot area. You may add multiple title above the plot area by
    #/ calling this method multiple times.
    #/ </summary>
    #/ <param name="alignment">The alignment with respect to the region that is on top of the
    #/ plot area.</param>
    #/ <param name="text">The text to add.</param>
    #/ <returns>The TextBox object representing the text box above the plot area.</returns>
    function addPlotAreaTitle($alignment, $text) {
        $ret = $this->addText($this->m_leftMargin, 0, $text, "bold", 10, TextColor, $alignment);
        $ret->setSize($this->m_totalWidth - $this->m_leftMargin - $this->m_rightMargin + 1,
            $this->m_topMargin - 1);
        $ret->setMargin(0);
        return $ret;
    }

    #/ <summary>
    #/ Set the plot area style. The default is to use pale yellow 0xfffff0 as the background,
    #/ and light grey 0xdddddd as the grid lines.
    #/ </summary>
    #/ <param name="bgColor">The plot area background color.</param>
    #/ <param name="majorHGridColor">Major horizontal grid color.</param>
    #/ <param name="majorVGridColor">Major vertical grid color.</param>
    #/ <param name="minorHGridColor">Minor horizontal grid color. In current version, minor
    #/ horizontal grid is not used.</param>
    #/ <param name="minorVGridColor">Minor vertical grid color.</param>
    function setPlotAreaStyle($bgColor, $majorHGridColor, $majorVGridColor, $minorHGridColor,
        $minorVGridColor) {
        $this->m_plotAreaBgColor = $bgColor;
        $this->m_majorHGridColor = $majorHGridColor;
        $this->m_majorVGridColor = $majorVGridColor;
        $this->m_minorHGridColor = $minorHGridColor;
        $this->m_minorVGridColor = $minorVGridColor;
    }

    #/ <summary>
    #/ Set the plot area border style. The default is grey color (888888), with a gap
    #/ of 2 pixels between charts.
    #/ </summary>
    #/ <param name="borderColor">The color of the border.</param>
    #/ <param name="borderGap">The gap between two charts.</param>
    function setPlotAreaBorder($borderColor, $borderGap) {
        $this->m_plotAreaBorder = $borderColor;
        $this->m_plotAreaGap = $borderGap;
    }

    #/ <summary>
    #/ Set legend style. The default is Arial 8 pt black color, with light grey background.
    #/ </summary>
    #/ <param name="font">The font of the legend text.</param>
    #/ <param name="fontSize">The font size of the legend text in points.</param>
    #/ <param name="fontColor">The color of the legend text.</param>
    #/ <param name="bgColor">The background color of the legend box.</param>
    function setLegendStyle($font, $fontSize, $fontColor, $bgColor) {
        $this->m_legendFont = $font;
        $this->m_legendFontSize = $fontSize;
        $this->m_legendFontColor = $fontColor;
        $this->m_legendBgColor = $bgColor;
    }

    #/ <summary>
    #/ Set x-axis label style. The default is Arial 8 pt black color no rotation.
    #/ </summary>
    #/ <param name="font">The font of the axis labels.</param>
    #/ <param name="fontSize">The font size of the axis labels in points.</param>
    #/ <param name="fontColor">The color of the axis labels.</param>
    #/ <param name="fontAngle">The rotation of the axis labels.</param>
    function setXAxisStyle($font, $fontSize, $fontColor, $fontAngle) {
        $this->m_xAxisFont = $font;
        $this->m_xAxisFontSize = $fontSize;
        $this->m_xAxisFontColor = $fontColor;
        $this->m_xAxisFontAngle = $fontAngle;
    }

    #/ <summary>
    #/ Set y-axis label style. The default is Arial 8 pt black color, with 13 pixels margin.
    #/ </summary>
    #/ <param name="font">The font of the axis labels.</param>
    #/ <param name="fontSize">The font size of the axis labels in points.</param>
    #/ <param name="fontColor">The color of the axis labels.</param>
    #/ <param name="axisMargin">The margin at the top of the y-axis in pixels (to leave
    #/ space for the legend box).</param>
    function setYAxisStyle($font, $fontSize, $fontColor, $axisMargin) {
        $this->m_yAxisFont = $font;
        $this->m_yAxisFontSize = $fontSize;
        $this->m_yAxisFontColor = $fontColor;
        $this->m_yAxisMargin = $axisMargin;
    }

    #/ <summary>
    #/ Set whether the main y-axis is on right of left side of the plot area. The default is
    #/ on right.
    #/ </summary>
    #/ <param name="b">True if the y-axis is on right. False if the y-axis is on left.</param>
    function setAxisOnRight($b) {
        $this->m_axisOnRight = $b;
    }

    #/ <summary>
    #/ Determines if log scale should be used for the main chart. The default is linear scale.
    #/ </summary>
    #/ <param name="b">True for using log scale. False for using linear scale.</param>
    function setLogScale($b) {
        $this->m_logScale = $b;
        if ($this->m_mainChart != null) {
            if ($this->m_logScale) {
                $this->m_mainChart->yAxis->setLogScale();
            } else {
                $this->m_mainChart->yAxis->setLinearScale();
            }
        }
    }

    #/ <summary>
    #/ Set the date/time formats to use for the x-axis labels under various cases.
    #/ </summary>
    #/ <param name="yearFormat">The format for displaying labels on an axis with yearly ticks. The
    #/ default is "yyyy".</param>
    #/ <param name="firstMonthFormat">The format for displaying labels on an axis with monthly ticks.
    #/ This parameter applies to the first available month of a year (usually January) only, so it can
    #/ be formatted differently from the other labels.</param>
    #/ <param name="otherMonthFormat">The format for displaying labels on an axis with monthly ticks.
    #/ This parameter applies to months other than the first available month of a year.</param>
    #/ <param name="firstDayFormat">The format for displaying labels on an axis with daily ticks.
    #/ This parameter applies to the first available day of a month only, so it can be formatted
    #/ differently from the other labels.</param>
    #/ <param name="otherDayFormat">The format for displaying labels on an axis with daily ticks.
    #/ This parameter applies to days other than the first available day of a month.</param>
    #/ <param name="firstHourFormat">The format for displaying labels on an axis with hourly
    #/ resolution. This parameter applies to the first tick of a day only, so it can be formatted
    #/ differently from the other labels.</param>
    #/ <param name="otherHourFormat">The format for displaying labels on an axis with hourly.
    #/ resolution. This parameter applies to ticks at hourly boundaries, except the first tick
    #/ of a day.</param>
    function setDateLabelFormat($yearFormat, $firstMonthFormat, $otherMonthFormat, $firstDayFormat,
        $otherDayFormat, $firstHourFormat, $otherHourFormat) {
        if ($yearFormat != null) {
            $this->m_yearFormat = $yearFormat;
        }
        if ($firstMonthFormat != null) {
            $this->m_firstMonthFormat = $firstMonthFormat;
        }
        if ($otherMonthFormat != null) {
            $this->m_otherMonthFormat = $otherMonthFormat;
        }
        if ($firstDayFormat != null) {
            $this->m_firstDayFormat = $firstDayFormat;
        }
        if ($otherDayFormat != null) {
            $this->m_otherDayFormat = $otherDayFormat;
        }
        if ($firstHourFormat != null) {
            $this->m_firstHourFormat = $firstHourFormat;
        }
        if ($otherHourFormat != null) {
            $this->m_otherHourFormat = $otherHourFormat;
        }
    }

    #/ <summary>
    #/ Set the minimum label spacing between two labels on the time axis
    #/ </summary>
    #/ <param name="labelSpacing">The minimum label spacing in pixels.</param>
    function setDateLabelSpacing($labelSpacing) {
        if ($labelSpacing > 0) {
            $this->m_timeLabelSpacing = $labelSpacing;
        } else {
             $this->m_timeLabelSpacing = 0;
        }
    }

    #/ <summary>
    #/ Set the tool tip formats for display date/time
    #/ </summary>
    #/ <param name="monthFormat">The tool tip format to use if the data point spacing is one
    #/ or more months (more than 30 days).</param>
    #/ <param name="dayFormat">The tool tip format to use if the data point spacing is 1 day
    #/ to less than 30 days.</param>
    #/ <param name="hourFormat">The tool tip format to use if the data point spacing is less
    #/ than 1 day.</param>
    function setToolTipDateFormat($monthFormat, $dayFormat, $hourFormat) {
        if ($monthFormat != null) {
            $this->m_toolTipMonthFormat = $monthFormat;
        }
        if ($dayFormat != null) {
            $this->m_toolTipDayFormat = $dayFormat;
        }
        if ($hourFormat != null) {
            $this->m_toolTipHourFormat = $hourFormat;
        }
    }

    #/ <summary>
    #/ Get the tool tip format for display date/time
    #/ </summary>
    #/ <returns>The tool tip format string.</returns>
    function getToolTipDateFormat() {
        if ($this->m_timeStamps == null) {
            return $this->m_toolTipHourFormat;
        }
        if (count($this->m_timeStamps) <= $this->m_extraPoints) {
            return $this->m_toolTipHourFormat;
        }
        $resolution = ($this->m_timeStamps[count($this->m_timeStamps) - 1] - $this->m_timeStamps[0])
             / count($this->m_timeStamps);
        if ($resolution >= 30 * 86400) {
            return $this->m_toolTipMonthFormat;
        } else if ($resolution >= 86400) {
            return $this->m_toolTipDayFormat;
        } else {
            return $this->m_toolTipHourFormat;
        }
    }

    #/ <summary>
    #/ Set the number format for use in displaying values in legend keys and tool tips.
    #/ </summary>
    #/ <param name="formatString">The default number format.</param>
    function setNumberLabelFormat($formatString) {
        if ($formatString != null) {
            $this->m_generalFormat = $formatString;
        }
    }

    #/ <summary>
    #/ A utility function to compute triangular moving averages
    #/ </summary>
    #/ <param name="data">An array of numbers as input.</param>
    #/ <param name="period">The moving average period.</param>
    #/ <returns>An array representing the triangular moving average of the input array.</returns>
    function computeTriMovingAvg($data, $period) {
        $p = $period / 2 + 1;
        return (new ArrayMath($data))->movAvg($p)->movAvg($p)->result();
    }

    #/ <summary>
    #/ A utility function to compute weighted moving averages
    #/ </summary>
    #/ <param name="data">An array of numbers as input.</param>
    #/ <param name="period">The moving average period.</param>
    #/ <returns>An array representing the weighted moving average of the input array.</returns>
    function computeWeightedMovingAvg($data, $period) {
        $acc = new ArrayMath($data);
        for($i = 2; $i < $period + 1; ++$i) {
            $acc->add((new ArrayMath($data))->movAvg($i)->mul($i)->result());
        }
        return $acc->div((1 + $period) * $period / 2)->result();
    }

    #/ <summary>
    #/ A utility function to obtain the first visible closing price.
    #/ </summary>
    #/ <returns>The first closing price.
    #/ are cd.NoValue.</returns>
    function firstCloseValue() {
        for($i = $this->m_extraPoints; $i < count($this->m_closeData); ++$i) {
            if (($this->m_closeData[$i] != NoValue) && ($this->m_closeData[$i] != 0)) {
                return $this->m_closeData[$i];
            }
        }
        return NoValue;
    }

    #/ <summary>
    #/ A utility function to obtain the last valid position (that is, position not
    #/ containing cd.NoValue) of a data series.
    #/ </summary>
    #/ <param name="data">An array of numbers as input.</param>
    #/ <returns>The last valid position in the input array, or -1 if all positions
    #/ are cd.NoValue.</returns>
    function lastIndex($data) {
        $i = count($data) - 1;
        while ($i >= 0) {
            if ($data[$i] != NoValue) {
                break;
            }
            $i = $i - 1;
        }
        return $i;
    }

    #/ <summary>
    #/ Set the data used in the chart. If some of the data are not available, some artifical
    #/ values should be used. For example, if the high and low values are not available, you
    #/ may use closeData as highData and lowData.
    #/ </summary>
    #/ <param name="timeStamps">An array of dates/times for the time intervals.</param>
    #/ <param name="highData">The high values in the time intervals.</param>
    #/ <param name="lowData">The low values in the time intervals.</param>
    #/ <param name="openData">The open values in the time intervals.</param>
    #/ <param name="closeData">The close values in the time intervals.</param>
    #/ <param name="volData">The volume values in the time intervals.</param>
    #/ <param name="extraPoints">The number of leading time intervals that are not
    #/ displayed in the chart. These intervals are typically used for computing
    #/ indicators that require extra leading data, such as moving averages.</param>
    function setData($timeStamps, $highData, $lowData, $openData, $closeData, $volData, $extraPoints
        ) {
        $this->m_timeStamps = $timeStamps;
        $this->m_highData = $highData;
        $this->m_lowData = $lowData;
        $this->m_openData = $openData;
        $this->m_closeData = $closeData;
        if ($extraPoints > 0) {
            $this->m_extraPoints = $extraPoints;
        } else {
            $this->m_extraPoints = 0;
        }

        #///////////////////////////////////////////////////////////////////////
        # Auto-detect volume units
        #///////////////////////////////////////////////////////////////////////
        $maxVol = (new ArrayMath($volData))->max();
        $units = array("", "K", "M", "B");
        $unitIndex = count($units) - 1;
        while (($unitIndex > 0) && ($maxVol < pow(1000, $unitIndex))) {
            $unitIndex = $unitIndex - 1;
        }

        $this->m_volData = (new ArrayMath($volData))->div(pow(1000, $unitIndex))->result();
        $this->m_volUnit = $units[$unitIndex];
    }

    #////////////////////////////////////////////////////////////////////////////
    # Format x-axis labels
    #////////////////////////////////////////////////////////////////////////////
    function setXLabels($a) {
        $a->setLabels2($this->m_timeStamps);
        if ($this->m_extraPoints < count($this->m_timeStamps)) {
            $tickStep = (int)((count($this->m_timeStamps) - $this->m_extraPoints) *
                $this->m_timeLabelSpacing / ($this->m_totalWidth - $this->m_leftMargin -
                $this->m_rightMargin)) + 1;
            $timeRangeInSeconds = $this->m_timeStamps[count($this->m_timeStamps) - 1] -
                $this->m_timeStamps[$this->m_extraPoints];
            $secondsBetweenTicks = $timeRangeInSeconds / ($this->m_totalWidth - $this->m_leftMargin
                 - $this->m_rightMargin) * $this->m_timeLabelSpacing;

            if ($secondsBetweenTicks * (count($this->m_timeStamps) - $this->m_extraPoints) <=
                $timeRangeInSeconds) {
                $tickStep = 1;
                if (count($this->m_timeStamps) > 1) {
                    $secondsBetweenTicks = $this->m_timeStamps[count($this->m_timeStamps) - 1] -
                        $this->m_timeStamps[count($this->m_timeStamps) - 2];
                } else {
                    $secondsBetweenTicks = 86400;
                }
            }

            if (($secondsBetweenTicks > 360 * 86400) || (($secondsBetweenTicks > 90 * 86400) && (
                $timeRangeInSeconds >= 720 * 86400))) {
                #yearly ticks
                $a->setMultiFormat2(StartOfYearFilter(), $this->m_yearFormat, $tickStep);
            } else if (($secondsBetweenTicks >= 30 * 86400) || (($secondsBetweenTicks > 7 * 86400)
                 && ($timeRangeInSeconds >= 60 * 86400))) {
                #monthly ticks
                $monthBetweenTicks = (int)($secondsBetweenTicks / 31 / 86400) + 1;
                $a->setMultiFormat(StartOfYearFilter(), $this->m_firstMonthFormat,
                    StartOfMonthFilter($monthBetweenTicks), $this->m_otherMonthFormat);
                $a->setMultiFormat2(StartOfMonthFilter(), "-", 1, false);
            } else if (($secondsBetweenTicks >= 86400) || (($secondsBetweenTicks > 6 * 3600) && (
                $timeRangeInSeconds >= 86400))) {
                #daily ticks
                $a->setMultiFormat(StartOfMonthFilter(), $this->m_firstDayFormat, StartOfDayFilter(
                    1, 0.5), $this->m_otherDayFormat, $tickStep);
            } else {
                #hourly ticks
                $a->setMultiFormat(StartOfDayFilter(1, 0.5), $this->m_firstHourFormat,
                    StartOfHourFilter(1, 0.5), $this->m_otherHourFormat, $tickStep);
            }
        }
    }

    #////////////////////////////////////////////////////////////////////////////
    # Create tool tip format string for showing OHLC data
    #////////////////////////////////////////////////////////////////////////////
    function getHLOCToolTipFormat() {
        return "title='" . $this->getToolTipDateFormat() . " Op:{open|" . $this->m_generalFormat .
            "}, Hi:{high|" . $this->m_generalFormat . "}, Lo:{low|" . $this->m_generalFormat .
            "}, Cl:{close|" . $this->m_generalFormat . "}'";
    }

    #/ <summary>
    #/ Add the main chart - the chart that shows the HLOC data.
    #/ </summary>
    #/ <param name="height">The height of the main chart in pixels.</param>
    #/ <returns>An XYChart object representing the main chart created.</returns>
    function addMainChart($height) {
        $this->m_mainChart = $this->addIndicator($height);
        $this->m_mainChart->yAxis->setMargin(2 * $this->m_yAxisMargin);
        if ($this->m_logScale) {
            $this->m_mainChart->yAxis->setLogScale();
        } else {
            $this->m_mainChart->yAxis->setLinearScale();
        }
        return $this->m_mainChart;
    }

    #/ <summary>
    #/ Add a candlestick layer to the main chart.
    #/ </summary>
    #/ <param name="upColor">The candle color for an up day.</param>
    #/ <param name="downColor">The candle color for a down day.</param>
    #/ <returns>The CandleStickLayer created.</returns>
    function addCandleStick($upColor, $downColor) {
        $this->addOHLCLabel($upColor, $downColor, true);
        $ret = $this->m_mainChart->addCandleStickLayer($this->m_highData, $this->m_lowData,
            $this->m_openData, $this->m_closeData, $upColor, $downColor);
        $ret->setHTMLImageMap("", "", $this->getHLOCToolTipFormat());
        if (count($this->m_highData) - $this->m_extraPoints > 60) {
            $ret->setDataGap(0);
        }

        if (count($this->m_highData) > $this->m_extraPoints) {
            $expectedWidth = (int)(($this->m_totalWidth - $this->m_leftMargin - $this->m_rightMargin
                ) / (count($this->m_highData) - $this->m_extraPoints));
            if ($expectedWidth <= 5) {
                $ret->setDataWidth($expectedWidth + 1 - $expectedWidth % 2);
            }
        }

        return $ret;
    }

    #/ <summary>
    #/ Add a HLOC layer to the main chart.
    #/ </summary>
    #/ <param name="upColor">The color of the HLOC symbol for an up day.</param>
    #/ <param name="downColor">The color of the HLOC symbol for a down day.</param>
    #/ <returns>The HLOCLayer created.</returns>
    function addHLOC($upColor, $downColor) {
        $this->addOHLCLabel($upColor, $downColor, false);
        $ret = $this->m_mainChart->addHLOCLayer($this->m_highData, $this->m_lowData,
            $this->m_openData, $this->m_closeData);
        $ret->setColorMethod(HLOCUpDown, $upColor, $downColor);
        $ret->setHTMLImageMap("", "", $this->getHLOCToolTipFormat());
        $ret->setDataGap(0);
        return $ret;
    }

    function addOHLCLabel($upColor, $downColor, $candleStickMode) {
        $i = $this->lastIndex($this->m_closeData);
        if ($i >= 0) {
            $openValue = NoValue;
            $closeValue = NoValue;
            $highValue = NoValue;
            $lowValue = NoValue;

            if ($i < count($this->m_openData)) {
                $openValue = $this->m_openData[$i];
            }
            if ($i < count($this->m_closeData)) {
                $closeValue = $this->m_closeData[$i];
            }
            if ($i < count($this->m_highData)) {
                $highValue = $this->m_highData[$i];
            }
            if ($i < count($this->m_lowData)) {
                $lowValue = $this->m_lowData[$i];
            }

            $openLabel = "";
            $closeLabel = "";
            $highLabel = "";
            $lowLabel = "";
            $delim = "";
            if ($openValue != NoValue) {
                $openLabel = "Op:" . $this->formatValue($openValue, $this->m_generalFormat);
                $delim = ", ";
            }
            if ($highValue != NoValue) {
                $highLabel = $delim . "Hi:" . $this->formatValue($highValue, $this->m_generalFormat)
                    ;
                $delim = ", ";
            }
            if ($lowValue != NoValue) {
                $lowLabel = $delim . "Lo:" . $this->formatValue($lowValue, $this->m_generalFormat);
                $delim = ", ";
            }
            if ($closeValue != NoValue) {
                $closeLabel = $delim . "Cl:" . $this->formatValue($closeValue,
                    $this->m_generalFormat);
                $delim = ", ";
            }
            $label = $openLabel . $highLabel . $lowLabel . $closeLabel;

            $useUpColor = ($closeValue >= $openValue);
            if ($candleStickMode != true) {
                $closeChanges = (new ArrayMath($this->m_closeData))->delta()->result();
                $lastChangeIndex = $this->lastIndex($closeChanges);
                $useUpColor = ($lastChangeIndex < 0);
                if ($useUpColor != true) {
                    $useUpColor = ($closeChanges[$lastChangeIndex] >= 0);
                }
            }

            $udcolor = $downColor;
            if ($useUpColor) {
                $udcolor = $upColor;
            }
            $this->m_mainChart->getLegend()->addKey($label, $udcolor);
        }
    }

    #/ <summary>
    #/ Add a closing price line on the main chart.
    #/ </summary>
    #/ <param name="color">The color of the line.</param>
    #/ <returns>The LineLayer object representing the line created.</returns>
    function addCloseLine($color) {
        return $this->addLineIndicator2($this->m_mainChart, $this->m_closeData, $color,
            "Closing Price");
    }

    #/ <summary>
    #/ Add a weight close line on the main chart.
    #/ </summary>
    #/ <param name="color">The color of the line.</param>
    #/ <returns>The LineLayer object representing the line created.</returns>
    function addWeightedClose($color) {
        return $this->addLineIndicator2($this->m_mainChart, (new ArrayMath($this->m_highData))->add(
            $this->m_lowData)->add($this->m_closeData)->add($this->m_closeData)->div(4)->result(),
            $color, "Weighted Close");
    }

    #/ <summary>
    #/ Add a typical price line on the main chart.
    #/ </summary>
    #/ <param name="color">The color of the line.</param>
    #/ <returns>The LineLayer object representing the line created.</returns>
    function addTypicalPrice($color) {
        return $this->addLineIndicator2($this->m_mainChart, (new ArrayMath($this->m_highData))->add(
            $this->m_lowData)->add($this->m_closeData)->div(3)->result(), $color, "Typical Price");
    }

    #/ <summary>
    #/ Add a median price line on the main chart.
    #/ </summary>
    #/ <param name="color">The color of the line.</param>
    #/ <returns>The LineLayer object representing the line created.</returns>
    function addMedianPrice($color) {
        return $this->addLineIndicator2($this->m_mainChart, (new ArrayMath($this->m_highData))->add(
            $this->m_lowData)->div(2)->result(), $color, "Median Price");
    }

    #/ <summary>
    #/ Add a simple moving average line on the main chart.
    #/ </summary>
    #/ <param name="period">The moving average period</param>
    #/ <param name="color">The color of the line.</param>
    #/ <returns>The LineLayer object representing the line created.</returns>
    function addSimpleMovingAvg($period, $color) {
        $label = "SMA (" . $period . ")";
        return $this->addLineIndicator2($this->m_mainChart, (new ArrayMath($this->m_closeData)
            )->movAvg($period)->result(), $color, $label);
    }

    #/ <summary>
    #/ Add an exponential moving average line on the main chart.
    #/ </summary>
    #/ <param name="period">The moving average period</param>
    #/ <param name="color">The color of the line.</param>
    #/ <returns>The LineLayer object representing the line created.</returns>
    function addExpMovingAvg($period, $color) {
        $label = "EMA (" . $period . ")";
        return $this->addLineIndicator2($this->m_mainChart, (new ArrayMath($this->m_closeData)
            )->expAvg(2.0 / ($period + 1))->result(), $color, $label);
    }

    #/ <summary>
    #/ Add a triangular moving average line on the main chart.
    #/ </summary>
    #/ <param name="period">The moving average period</param>
    #/ <param name="color">The color of the line.</param>
    #/ <returns>The LineLayer object representing the line created.</returns>
    function addTriMovingAvg($period, $color) {
        $label = "TMA (" . $period . ")";
        return $this->addLineIndicator2($this->m_mainChart, (new ArrayMath(
            $this->computeTriMovingAvg($this->m_closeData, $period)))->result(), $color, $label);
    }

    #/ <summary>
    #/ Add a weighted moving average line on the main chart.
    #/ </summary>
    #/ <param name="period">The moving average period</param>
    #/ <param name="color">The color of the line.</param>
    #/ <returns>The LineLayer object representing the line created.</returns>
    function addWeightedMovingAvg($period, $color) {
        $label = "WMA (" . $period . ")";
        return $this->addLineIndicator2($this->m_mainChart, (new ArrayMath(
            $this->computeWeightedMovingAvg($this->m_closeData, $period)))->result(), $color, $label
            );
    }

    #/ <summary>
    #/ Add a parabolic SAR indicator to the main chart.
    #/ </summary>
    #/ <param name="accInitial">Initial acceleration factor</param>
    #/ <param name="accIncrement">Acceleration factor increment</param>
    #/ <param name="accMaximum">Maximum acceleration factor</param>
    #/ <param name="symbolType">The symbol used to plot the parabolic SAR</param>
    #/ <param name="symbolSize">The symbol size in pixels</param>
    #/ <param name="fillColor">The fill color of the symbol</param>
    #/ <param name="edgeColor">The edge color of the symbol</param>
    #/ <returns>The LineLayer object representing the layer created.</returns>
    function addParabolicSAR($accInitial, $accIncrement, $accMaximum, $symbolType, $symbolSize,
        $fillColor, $edgeColor) {
        $isLong = true;
        $acc = $accInitial;
        $extremePoint = 0;
        $psar = array_fill(0, count($this->m_lowData), 0);

        $i_1 = -1;
        $i_2 = -1;

        for($i = 0; $i < count($this->m_lowData); ++$i) {
            $psar[$i] = NoValue;
            if (($this->m_lowData[$i] != NoValue) && ($this->m_highData[$i] != NoValue)) {
                if (($i_1 >= 0) && ($i_2 < 0)) {
                    if ($this->m_lowData[$i_1] <= $this->m_lowData[$i]) {
                        $psar[$i] = $this->m_lowData[$i_1];
                        $isLong = true;
                        if ($this->m_highData[$i_1] > $this->m_highData[$i]) {
                            $extremePoint = $this->m_highData[$i_1];
                        } else {
                            $extremePoint = $this->m_highData[$i];
                        }
                    } else {
                        $extremePoint = $this->m_lowData[$i];
                        $isLong = false;
                        if ($this->m_highData[$i_1] > $this->m_highData[$i]) {
                            $psar[$i] = $this->m_highData[$i_1];
                        } else {
                            $psar[$i] = $this->m_highData[$i];
                        }
                    }
                } else if (($i_1 >= 0) && ($i_2 >= 0)) {
                    if ($acc > $accMaximum) {
                        $acc = $accMaximum;
                    }

                    $psar[$i] = $psar[$i_1] + $acc * ($extremePoint - $psar[$i_1]);

                    if ($isLong) {
                        if ($this->m_lowData[$i] < $psar[$i]) {
                            $isLong = false;
                            $psar[$i] = $extremePoint;
                            $extremePoint = $this->m_lowData[$i];
                            $acc = $accInitial;
                        } else {
                            if ($this->m_highData[$i] > $extremePoint) {
                                $extremePoint = $this->m_highData[$i];
                                $acc = $acc + $accIncrement;
                            }

                            if ($this->m_lowData[$i_1] < $psar[$i]) {
                                $psar[$i] = $this->m_lowData[$i_1];
                            }
                            if ($this->m_lowData[$i_2] < $psar[$i]) {
                                $psar[$i] = $this->m_lowData[$i_2];
                            }
                        }
                    } else {
                        if ($this->m_highData[$i] > $psar[$i]) {
                            $isLong = true;
                            $psar[$i] = $extremePoint;
                            $extremePoint = $this->m_highData[$i];
                            $acc = $accInitial;
                        } else {
                            if ($this->m_lowData[$i] < $extremePoint) {
                                $extremePoint = $this->m_lowData[$i];
                                $acc = $acc + $accIncrement;
                            }

                            if ($this->m_highData[$i_1] > $psar[$i]) {
                                $psar[$i] = $this->m_highData[$i_1];
                            }
                            if ($this->m_highData[$i_2] > $psar[$i]) {
                                $psar[$i] = $this->m_highData[$i_2];
                            }
                        }
                    }
                }

                $i_2 = $i_1;
                $i_1 = $i;
            }
        }

        $ret = $this->addLineIndicator2($this->m_mainChart, $psar, $fillColor, "Parabolic SAR");
        $ret->setLineWidth(0);

        $ret = $this->addLineIndicator2($this->m_mainChart, $psar, $fillColor, "");
        $ret->setLineWidth(0);
        $ret->getDataSet(0)->setDataSymbol($symbolType, $symbolSize, $fillColor, $edgeColor);
        return $ret;
    }

    #/ <summary>
    #/ Add a comparison line to the main price chart.
    #/ </summary>
    #/ <param name="data">The data series to compare to</param>
    #/ <param name="color">The color of the comparison line</param>
    #/ <param name="name">The name of the comparison line</param>
    #/ <returns>The LineLayer object representing the line layer created.</returns>
    function addComparison($data, $color, $name) {
        $firstIndex = $this->m_extraPoints;
        while (($firstIndex < count($data)) && ($firstIndex < count($this->m_closeData))) {
            if (($data[$firstIndex] != NoValue) && ($this->m_closeData[$firstIndex] != NoValue) && (
                $data[$firstIndex] != 0) && ($this->m_closeData[$firstIndex] != 0)) {
                break;
            }
            $firstIndex = $firstIndex + 1;
        }
        if (($firstIndex >= count($data)) || ($firstIndex >= count($this->m_closeData))) {
            return null;
        }

        $scaleFactor = $this->m_closeData[$firstIndex] / $data[$firstIndex];
        $layer = $this->m_mainChart->addLineLayer((new ArrayMath($data))->mul($scaleFactor)->result(
            ), Transparent);
        $layer->setHTMLImageMap("{disable}");

        $a = $this->m_mainChart->addAxis(Right, 0);
        $a->setColors(Transparent, Transparent);
        $a->syncAxis($this->m_mainChart->yAxis, 1 / $scaleFactor, 0);

        $ret = $this->addLineIndicator2($this->m_mainChart, $data, $color, $name);
        $ret->setUseYAxis($a);
        return $ret;
    }

    #/ <summary>
    #/ Display percentage axis scale
    #/ </summary>
    #/ <returns>The Axis object representing the percentage axis.</returns>
    function setPercentageAxis() {
        $firstClose = $this->firstCloseValue();
        if ($firstClose == NoValue) {
            return null;
        }

        $axisAlign = Left;
        if ($this->m_axisOnRight) {
            $axisAlign = Right;
        }

        $ret = $this->m_mainChart->addAxis($axisAlign, 0);
        $this->configureYAxis($ret, 300);
        $ret->syncAxis($this->m_mainChart->yAxis, 100 / $firstClose);
        $ret->setRounding(false, false);
        $ret->setLabelFormat("{={value}-100|@}%");
        $this->m_mainChart->yAxis->setColors(Transparent, Transparent);
        $this->m_mainChart->getPlotArea()->setGridAxis(null, $ret);
        return $ret;
    }

    #/ <summary>
    #/ Add a generic band to the main finance chart. This method is used internally by other methods to add
    #/ various bands (eg. Bollinger band, Donchian channels, etc).
    #/ </summary>
    #/ <param name="upperLine">The data series for the upper band line.</param>
    #/ <param name="lowerLine">The data series for the lower band line.</param>
    #/ <param name="lineColor">The color of the upper and lower band line.</param>
    #/ <param name="fillColor">The color to fill the region between the upper and lower band lines.</param>
    #/ <param name="name">The name of the band.</param>
    #/ <returns>An InterLineLayer object representing the filled region.</returns>
    function addBand($upperLine, $lowerLine, $lineColor, $fillColor, $name) {
        $i = count($upperLine) - 1;
        if ($i >= count($lowerLine)) {
            $i = count($lowerLine) - 1;
        }

        while ($i >= 0) {
            if (($upperLine[$i] != NoValue) && ($lowerLine[$i] != NoValue)) {
                $name = $name . ": " . $this->formatValue($lowerLine[$i], $this->m_generalFormat) .
                    " - " . $this->formatValue($upperLine[$i], $this->m_generalFormat);
                break;
            }
            $i = $i - 1;
        }

        $layer = $this->m_mainChart->addLineLayer2();
        $layer->addDataSet($upperLine, $lineColor, $name);
        $layer->addDataSet($lowerLine, $lineColor);
        return $this->m_mainChart->addInterLineLayer($layer->getLine(0), $layer->getLine(1),
            $fillColor);
    }

    #/ <summary>
    #/ Add a Bollinger band on the main chart.
    #/ </summary>
    #/ <param name="period">The period to compute the band.</param>
    #/ <param name="bandWidth">The half-width of the band in terms multiples of standard deviation. Typically 2 is used.</param>
    #/ <param name="lineColor">The color of the lines defining the upper and lower limits.</param>
    #/ <param name="fillColor">The color to fill the regional within the band.</param>
    #/ <returns>The InterLineLayer object representing the band created.</returns>
    function addBollingerBand($period, $bandWidth, $lineColor, $fillColor) {
        #Bollinger Band is moving avg +/- (width * moving std deviation)
        $stdDev = (new ArrayMath($this->m_closeData))->movStdDev($period)->mul($bandWidth)->result()
            ;
        $movAvg = (new ArrayMath($this->m_closeData))->movAvg($period)->result();
        $label = "Bollinger (" . $period . ", " . $bandWidth . ")";
        return $this->addBand((new ArrayMath($movAvg))->add($stdDev)->result(), (new ArrayMath(
            $movAvg))->sub($stdDev)->selectGTZ(null, 0)->result(), $lineColor, $fillColor, $label);
    }

    #/ <summary>
    #/ Add a Donchian channel on the main chart.
    #/ </summary>
    #/ <param name="period">The period to compute the band.</param>
    #/ <param name="lineColor">The color of the lines defining the upper and lower limits.</param>
    #/ <param name="fillColor">The color to fill the regional within the band.</param>
    #/ <returns>The InterLineLayer object representing the band created.</returns>
    function addDonchianChannel($period, $lineColor, $fillColor) {
        #Donchian Channel is the zone between the moving max and moving min
        $label = "Donchian (" . $period . ")";
        return $this->addBand((new ArrayMath($this->m_highData))->movMax($period)->result(), (
            new ArrayMath($this->m_lowData))->movMin($period)->result(), $lineColor, $fillColor,
            $label);
    }

    #/ <summary>
    #/ Add a price envelop on the main chart. The price envelop is a defined as a ratio around a
    #/ moving average. For example, a ratio of 0.2 means 20% above and below the moving average.
    #/ </summary>
    #/ <param name="period">The period for the moving average.</param>
    #/ <param name="range">The ratio above and below the moving average.</param>
    #/ <param name="lineColor">The color of the lines defining the upper and lower limits.</param>
    #/ <param name="fillColor">The color to fill the regional within the band.</param>
    #/ <returns>The InterLineLayer object representing the band created.</returns>
    function addEnvelop($period, $range, $lineColor, $fillColor) {
        #Envelop is moving avg +/- percentage
        $movAvg = (new ArrayMath($this->m_closeData))->movAvg($period)->result();
        $label = "Envelop (SMA " . $period . " +/- " . (int)($range * 100) . "%)";
        return $this->addBand((new ArrayMath($movAvg))->mul(1 + $range)->result(), (new ArrayMath(
            $movAvg))->mul(1 - $range)->result(), $lineColor, $fillColor, $label);
    }

    #/ <summary>
    #/ Add a volume bar chart layer on the main chart.
    #/ </summary>
    #/ <param name="height">The height of the bar chart layer in pixels.</param>
    #/ <param name="upColor">The color to used on an 'up' day. An 'up' day is a day where
    #/ the closing price is higher than that of the previous day.</param>
    #/ <param name="downColor">The color to used on a 'down' day. A 'down' day is a day
    #/ where the closing price is lower than that of the previous day.</param>
    #/ <param name="flatColor">The color to used on a 'flat' day. A 'flat' day is a day
    #/ where the closing price is the same as that of the previous day.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addVolBars($height, $upColor, $downColor, $flatColor) {
        return $this->addVolBars2($this->m_mainChart, $height, $upColor, $downColor, $flatColor);
    }

    function addVolBars2($c, $height, $upColor, $downColor, $flatColor) {
        $barLayer = $c->addBarLayer2(Overlay);
        $barLayer->setBorderColor(Transparent);

        if ($c == $this->m_mainChart) {
            $this->configureYAxis($c->yAxis2, $height);
            $topMargin = $c->getDrawArea()->getHeight() - $this->m_topMargin - $this->m_bottomMargin
                 - $height + $this->m_yAxisMargin;
            if ($topMargin < 0) {
                $topMargin = 0;
            }
            $c->yAxis2->setTopMargin($topMargin);
            $barLayer->setUseYAxis2();
        }

        $a = $c->yAxis2;
        if ($c != $this->m_mainChart) {
            $a = $c->yAxis;
        }
        if ((new ArrayMath($this->m_volData))->max() < 10) {
            $a->setLabelFormat("{value|1}" . $this->m_volUnit);
        } else {
            $a->setLabelFormat("{value}" . $this->m_volUnit);
        }

        $closeChange = (new ArrayMath($this->m_closeData))->delta()->replace(NoValue, 0)->result();
        $i = $this->lastIndex($this->m_volData);
        $label = "Vol";
        if ($i >= 0) {
            $label = $label . ": " . $this->formatValue($this->m_volData[$i], $this->m_generalFormat
                ) . $this->m_volUnit;
        }

        $upDS = $barLayer->addDataSet((new ArrayMath($this->m_volData))->selectGTZ($closeChange
            )->result(), $upColor);
        $dnDS = $barLayer->addDataSet((new ArrayMath($this->m_volData))->selectLTZ($closeChange
            )->result(), $downColor);
        $flatDS = $barLayer->addDataSet((new ArrayMath($this->m_volData))->selectEQZ($closeChange
            )->result(), $flatColor);

        if (($i < 0) || ($closeChange[$i] == 0) || ($closeChange[$i] == NoValue)) {
            $flatDS->setDataName($label);
        } else if ($closeChange[$i] > 0) {
            $upDS->setDataName($label);
        } else {
            $dnDS->setDataName($label);
        }

        return $barLayer;
    }

    #/ <summary>
    #/ Add a blank indicator chart to the finance chart. Used internally to add other indicators.
    #/ Override to change the default formatting (eg. axis fonts, etc.) of the various indicators.
    #/ </summary>
    #/ <param name="height">The height of the chart in pixels.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addIndicator($height) {
        #create a new chart object
        $ret = new XYChart($this->m_totalWidth, $height + $this->m_topMargin +
            $this->m_bottomMargin, Transparent);
        $ret->setTrimData($this->m_extraPoints);

        if ($this->m_currentChart != null) {
            #if there is a chart before the newly created chart, disable its x-axis, and copy
            #its x-axis labels to the new chart
            $this->m_currentChart->xAxis->setColors(Transparent, Transparent);
            $ret->xAxis->copyAxis($this->m_currentChart->xAxis);

            #add chart to MultiChart and update the total height
            $this->addChart(0, $this->m_totalHeight + $this->m_plotAreaGap, $ret);
            $this->m_totalHeight = $this->m_totalHeight + $height + 1 + $this->m_plotAreaGap;
        } else {
            #no existing chart - create the x-axis labels from scratch
            $this->setXLabels($ret->xAxis);

            #add chart to MultiChart and update the total height
            $this->addChart(0, $this->m_totalHeight, $ret);
            $this->m_totalHeight = $this->m_totalHeight + $height + 1;
        }

        #the newly created chart becomes the current chart
        $this->m_currentChart = $ret;

        #update the size
        $this->setSize($this->m_totalWidth, $this->m_totalHeight + $this->m_topMargin +
            $this->m_bottomMargin);

        #configure the plot area
        $ret->setPlotArea($this->m_leftMargin, $this->m_topMargin, $this->m_totalWidth -
            $this->m_leftMargin - $this->m_rightMargin, $height, $this->m_plotAreaBgColor, -1,
            $this->m_plotAreaBorder)->setGridColor($this->m_majorHGridColor,
            $this->m_majorVGridColor, $this->m_minorHGridColor, $this->m_minorVGridColor);
        $ret->setAntiAlias($this->m_antiAlias);

        #configure legend box
        if ($this->m_legendFontColor != Transparent) {
            $box = $ret->addLegend($this->m_leftMargin, $this->m_topMargin, false,
                $this->m_legendFont, $this->m_legendFontSize);
            $box->setFontColor($this->m_legendFontColor);
            $box->setBackground($this->m_legendBgColor);
            $box->setMargin2(5, 0, 2, 1);
            $box->setSize($this->m_totalWidth - $this->m_leftMargin - $this->m_rightMargin + 1, 0);
        }

        #configure x-axis
        $a = $ret->xAxis;
        $a->setIndent(true);
        $a->setTickLength(2, 0);
        $a->setColors(Transparent, $this->m_xAxisFontColor, $this->m_xAxisFontColor,
            $this->m_xAxisFontColor);
        $a->setLabelStyle($this->m_xAxisFont, $this->m_xAxisFontSize, $this->m_xAxisFontColor,
            $this->m_xAxisFontAngle);

        #configure y-axis
        $ret->setYAxisOnRight($this->m_axisOnRight);
        $this->configureYAxis($ret->yAxis, $height);

        return $ret;
    }

    function configureYAxis($a, $height) {
        $a->setAutoScale(0, 0.05, 0);
        if ($height < 100) {
            $a->setTickDensity(15);
        }
        $a->setMargin($this->m_yAxisMargin);
        $a->setLabelStyle($this->m_yAxisFont, $this->m_yAxisFontSize, $this->m_yAxisFontColor, 0);
        $a->setTickLength(-4, -2);
        $a->setColors(Transparent, $this->m_yAxisFontColor, $this->m_yAxisFontColor,
            $this->m_yAxisFontColor);
    }

    #/ <summary>
    #/ Add a generic line indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="data">The data series of the indicator line.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="name">The name of the indicator.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addLineIndicator($height, $data, $color, $name) {
        $c = $this->addIndicator($height);
        $this->addLineIndicator2($c, $data, $color, $name);
        return $c;
    }

    #/ <summary>
    #/ Add a line to an existing indicator chart.
    #/ </summary>
    #/ <param name="c">The indicator chart to add the line to.</param>
    #/ <param name="data">The data series of the indicator line.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="name">The name of the indicator.</param>
    #/ <returns>The LineLayer object representing the line created.</returns>
    function addLineIndicator2($c, $data, $color, $name) {
        return $c->addLineLayer($data, $color, $this->formatIndicatorLabel($name, $data));
    }

    #/ <summary>
    #/ Add a generic bar indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="data">The data series of the indicator bars.</param>
    #/ <param name="color">The color of the indicator bars.</param>
    #/ <param name="name">The name of the indicator.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addBarIndicator($height, $data, $color, $name) {
        $c = $this->addIndicator($height);
        $this->addBarIndicator2($c, $data, $color, $name);
        return $c;
    }

    #/ <summary>
    #/ Add a bar layer to an existing indicator chart.
    #/ </summary>
    #/ <param name="c">The indicator chart to add the bar layer to.</param>
    #/ <param name="data">The data series of the indicator bars.</param>
    #/ <param name="color">The color of the indicator bars.</param>
    #/ <param name="name">The name of the indicator.</param>
    #/ <returns>The BarLayer object representing the bar layer created.</returns>
    function addBarIndicator2($c, $data, $color, $name) {
        $layer = $c->addBarLayer($data, $color, $this->formatIndicatorLabel($name, $data));
        $layer->setBorderColor(Transparent);
        return $layer;
    }

    #/ <summary>
    #/ Add an upper/lower threshold range to an existing indicator chart.
    #/ </summary>
    #/ <param name="c">The indicator chart to add the threshold range to.</param>
    #/ <param name="layer">The line layer that the threshold range applies to.</param>
    #/ <param name="topRange">The upper threshold.</param>
    #/ <param name="topColor">The color to fill the region of the line that is above the
    #/ upper threshold.</param>
    #/ <param name="bottomRange">The lower threshold.</param>
    #/ <param name="bottomColor">The color to fill the region of the line that is below
    #/ the lower threshold.</param>
    function addThreshold($c, $layer, $topRange, $topColor, $bottomRange, $bottomColor) {
        $topMark = $c->yAxis->addMark($topRange, $topColor, $this->formatValue($topRange,
            $this->m_generalFormat));
        $bottomMark = $c->yAxis->addMark($bottomRange, $bottomColor, $this->formatValue(
            $bottomRange, $this->m_generalFormat));

        $c->addInterLineLayer($layer->getLine(), $topMark->getLine(), $topColor, Transparent);
        $c->addInterLineLayer($layer->getLine(), $bottomMark->getLine(), Transparent, $bottomColor);
    }

    function formatIndicatorLabel($name, $data) {
        $i = $this->lastIndex($data);
        if ($name == null) {
            return $name;
        }
        if (($name == "") || ($i < 0)) {
            return $name;
        }
        $ret = $name . ": " . $this->formatValue($data[$i], $this->m_generalFormat);
        return $ret;
    }

    #/ <summary>
    #/ Add an Accumulation/Distribution indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addAccDist($height, $color) {
        #Close Location Value = ((C - L) - (H - C)) / (H - L)
        #Accumulation Distribution Line = Accumulation of CLV * volume
        $range = (new ArrayMath($this->m_highData))->sub($this->m_lowData)->result();
        return $this->addLineIndicator($height, (new ArrayMath($this->m_closeData))->mul(2)->sub(
            $this->m_lowData)->sub($this->m_highData)->mul($this->m_volData)->financeDiv($range, 0
            )->acc()->result(), $color, "Accumulation/Distribution");
    }

    function computeAroonUp($period) {
        $aroonUp = array_fill(0, count($this->m_highData), 0);
        for($i = 0; $i < count($this->m_highData); ++$i) {
            $highValue = $this->m_highData[$i];
            if ($highValue == NoValue) {
                $aroonUp[$i] = NoValue;
            } else {
                $currentIndex = $i;
                $highCount = $period;
                $count = $period;

                while (($count > 0) && ($currentIndex >= $count)) {
                    $currentIndex = $currentIndex - 1;
                    $currentValue = $this->m_highData[$currentIndex];
                    if ($currentValue != NoValue) {
                        $count = $count - 1;
                        if ($currentValue > $highValue) {
                            $highValue = $currentValue;
                            $highCount = $count;
                        }
                    }
                }

                if ($count > 0) {
                    $aroonUp[$i] = NoValue;
                } else {
                    $aroonUp[$i] = $highCount * 100.0 / $period;
                }
            }
        }

        return $aroonUp;
    }

    function computeAroonDn($period) {
        $aroonDn = array_fill(0, count($this->m_lowData), 0);
        for($i = 0; $i < count($this->m_lowData); ++$i) {
            $lowValue = $this->m_lowData[$i];
            if ($lowValue == NoValue) {
                $aroonDn[$i] = NoValue;
            } else {
                $currentIndex = $i;
                $lowCount = $period;
                $count = $period;

                while (($count > 0) && ($currentIndex >= $count)) {
                    $currentIndex = $currentIndex - 1;
                    $currentValue = $this->m_lowData[$currentIndex];
                    if ($currentValue != NoValue) {
                        $count = $count - 1;
                        if ($currentValue < $lowValue) {
                            $lowValue = $currentValue;
                            $lowCount = $count;
                        }
                    }
                }

                if ($count > 0) {
                    $aroonDn[$i] = NoValue;
                } else {
                    $aroonDn[$i] = $lowCount * 100.0 / $period;
                }
            }
        }

        return $aroonDn;
    }

    #/ <summary>
    #/ Add an Aroon Up/Down indicators chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicators.</param>
    #/ <param name="upColor">The color of the Aroon Up indicator line.</param>
    #/ <param name="downColor">The color of the Aroon Down indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addAroon($height, $period, $upColor, $downColor) {
        $c = $this->addIndicator($height);
        $this->addLineIndicator2($c, $this->computeAroonUp($period), $upColor, "Aroon Up");
        $this->addLineIndicator2($c, $this->computeAroonDn($period), $downColor, "Aroon Down");
        $c->yAxis->setLinearScale(0, 100);
        return $c;
    }

    #/ <summary>
    #/ Add an Aroon Oscillator indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addAroonOsc($height, $period, $color) {
        $label = "Aroon Oscillator (" . $period . ")";
        $c = $this->addLineIndicator($height, (new ArrayMath($this->computeAroonUp($period)))->sub(
            $this->computeAroonDn($period))->result(), $color, $label);
        $c->yAxis->setLinearScale(-100, 100);
        return $c;
    }

    function computeTrueRange() {
        $previousClose = (new ArrayMath($this->m_closeData))->shift()->result();
        $ret = (new ArrayMath($this->m_highData))->sub($this->m_lowData)->result();
        $temp = 0;

        for($i = 0; $i < count($this->m_highData); ++$i) {
            if (($ret[$i] != NoValue) && ($previousClose[$i] != NoValue)) {
                $temp = abs($this->m_highData[$i] - $previousClose[$i]);
                if ($temp > $ret[$i]) {
                    $ret[$i] = $temp;
                }
                $temp = abs($previousClose[$i] - $this->m_lowData[$i]);
                if ($temp > $ret[$i]) {
                    $ret[$i] = $temp;
                }
            }
        }

        return $ret;
    }

    #/ <summary>
    #/ Add an Average Directional Index indicators chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="posColor">The color of the Positive Directional Index line.</param>
    #/ <param name="negColor">The color of the Negatuve Directional Index line.</param>
    #/ <param name="color">The color of the Average Directional Index line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addADX($height, $period, $posColor, $negColor, $color) {
        #pos/neg directional movement
        $pos = (new ArrayMath($this->m_highData))->delta()->selectGTZ();
        $neg = (new ArrayMath($this->m_lowData))->delta()->mul(-1)->selectGTZ();
        $delta = (new ArrayMath($pos->result()))->sub($neg->result())->result();
        $pos->selectGTZ($delta);
        $neg->selectLTZ($delta);

        #initial value
        $posData = $pos->result();
        $negData = $neg->result();
        if ((count($posData) > 1) && (abs($posData[1] / NoValue - 1) > 1e-05) && (abs($negData[1] /
            NoValue - 1) > 1e-05)) {
            $posData[1] = ($posData[1] * 2 + $negData[1]) / 3;
            $negData[1] = ($negData[1] + $posData[1]) / 2;
            $pos = new ArrayMath($posData);
            $neg = new ArrayMath($negData);
        }

        #pos/neg directional index
        $tr = $this->computeTrueRange();
        $tr = (new ArrayMath($tr))->expAvg(1.0 / $period)->result();
        $pos->expAvg(1.0 / $period)->financeDiv($tr, 0)->mul(100);
        $neg->expAvg(1.0 / $period)->financeDiv($tr, 0)->mul(100);

        #directional movement index ??? what happen if division by zero???
        $totalDM = (new ArrayMath($pos->result()))->add($neg->result())->result();
        $dx = (new ArrayMath($pos->result()))->sub($neg->result())->abs()->financeDiv($totalDM, 0
            )->mul(100)->expAvg(1.0 / $period);

        $c = $this->addIndicator($height);
        $label1 = "+DI (" . $period . ")";
        $label2 = "-DI (" . $period . ")";
        $label3 = "ADX (" . $period . ")";
        $this->addLineIndicator2($c, $pos->result(), $posColor, $label1);
        $this->addLineIndicator2($c, $neg->result(), $negColor, $label2);
        $this->addLineIndicator2($c, $dx->result(), $color, $label3);
        return $c;
    }

    #/ <summary>
    #/ Add an Average True Range indicators chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color1">The color of the True Range line.</param>
    #/ <param name="color2">The color of the Average True Range line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addATR($height, $period, $color1, $color2) {
        $trueRange = $this->computeTrueRange();
        $c = $this->addLineIndicator($height, $trueRange, $color1, "True Range");
        $label = "Average True Range (" . $period . ")";
        $this->addLineIndicator2($c, (new ArrayMath($trueRange))->expAvg(2.0 / ($period + 1)
            )->result(), $color2, $label);
        return $c;
    }

    #/ <summary>
    #/ Add a Bollinger Band Width indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="width">The band width to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addBollingerWidth($height, $period, $width, $color) {
        $label = "Bollinger Width (" . $period . ", " . $width . ")";
        return $this->addLineIndicator($height, (new ArrayMath($this->m_closeData))->movStdDev(
            $period)->mul($width * 2)->result(), $color, $label);
    }

    #/ <summary>
    #/ Add a Community Channel Index indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="deviation">The distance beween the middle line and the upper and lower threshold lines.</param>
    #/ <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    #/ <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addCCI($height, $period, $color, $deviation, $upColor, $downColor) {
        #typical price
        $tp = (new ArrayMath($this->m_highData))->add($this->m_lowData)->add($this->m_closeData
            )->div(3)->result();

        #simple moving average of typical price
        $smvtp = (new ArrayMath($tp))->movAvg($period)->result();

        #compute mean deviation
        $movMeanDev = array_fill(0, count($smvtp), 0);
        for($i = 0; $i < count($smvtp); ++$i) {
            $avg = $smvtp[$i];
            if ($avg == NoValue) {
                $movMeanDev[$i] = NoValue;
            } else {
                $currentIndex = $i;
                $count = $period - 1;
                $acc = 0;

                while (($count >= 0) && ($currentIndex >= $count)) {
                    $currentValue = $tp[$currentIndex];
                    $currentIndex = $currentIndex - 1;
                    if ($currentValue != NoValue) {
                        $count = $count - 1;
                        $acc = $acc + abs($avg - $currentValue);
                    }
                }

                if ($count > 0) {
                    $movMeanDev[$i] = NoValue;
                } else {
                    $movMeanDev[$i] = $acc / $period;
                }
            }
        }

        $c = $this->addIndicator($height);
        $label = "CCI (" . $period . ")";
        $layer = $this->addLineIndicator2($c, (new ArrayMath($tp))->sub($smvtp)->financeDiv(
            $movMeanDev, 0)->div(0.015)->result(), $color, $label);
        $this->addThreshold($c, $layer, $deviation, $upColor, -$deviation, $downColor);
        return $c;
    }

    #/ <summary>
    #/ Add a Chaikin Money Flow indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addChaikinMoneyFlow($height, $period, $color) {
        $range = (new ArrayMath($this->m_highData))->sub($this->m_lowData)->result();
        $volAvg = (new ArrayMath($this->m_volData))->movAvg($period)->result();
        $label = "Chaikin Money Flow (" . $period . ")";
        return $this->addBarIndicator($height, (new ArrayMath($this->m_closeData))->mul(2)->sub(
            $this->m_lowData)->sub($this->m_highData)->mul($this->m_volData)->financeDiv($range, 0
            )->movAvg($period)->financeDiv($volAvg, 0)->result(), $color, $label);
    }

    #/ <summary>
    #/ Add a Chaikin Oscillator indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addChaikinOscillator($height, $color) {
        #first compute acc/dist line
        $range = (new ArrayMath($this->m_highData))->sub($this->m_lowData)->result();
        $accdist = (new ArrayMath($this->m_closeData))->mul(2)->sub($this->m_lowData)->sub(
            $this->m_highData)->mul($this->m_volData)->financeDiv($range, 0)->acc()->result();

        #chaikin osc = exp3(accdist) - exp10(accdist)
        $expAvg10 = (new ArrayMath($accdist))->expAvg(2.0 / (10 + 1))->result();
        return $this->addLineIndicator($height, (new ArrayMath($accdist))->expAvg(2.0 / (3 + 1)
            )->sub($expAvg10)->result(), $color, "Chaikin Oscillator");
    }

    #/ <summary>
    #/ Add a Chaikin Volatility indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period1">The period to smooth the range.</param>
    #/ <param name="period2">The period to compute the rate of change of the smoothed range.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addChaikinVolatility($height, $period1, $period2, $color) {
        $label = "Chaikin Volatility (" . $period1 . ", " . $period2 . ")";
        return $this->addLineIndicator($height, (new ArrayMath($this->m_highData))->sub(
            $this->m_lowData)->expAvg(2.0 / ($period1 + 1))->rate($period2)->sub(1)->mul(100
            )->result(), $color, $label);
    }

    #/ <summary>
    #/ Add a Close Location Value indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addCLV($height, $color) {
        #Close Location Value = ((C - L) - (H - C)) / (H - L)
        $range = (new ArrayMath($this->m_highData))->sub($this->m_lowData)->result();
        return $this->addLineIndicator($height, (new ArrayMath($this->m_closeData))->mul(2)->sub(
            $this->m_lowData)->sub($this->m_highData)->financeDiv($range, 0)->result(), $color,
            "Close Location Value");
    }

    #/ <summary>
    #/ Add a Detrended Price Oscillator indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addDPO($height, $period, $color) {
        $label = "DPO (" . $period . ")";
        return $this->addLineIndicator($height, (new ArrayMath($this->m_closeData))->movAvg($period
            )->shift($period / 2 + 1)->sub($this->m_closeData)->mul(-1)->result(), $color, $label);
    }

    #/ <summary>
    #/ Add a Donchian Channel Width indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addDonchianWidth($height, $period, $color) {
        $label = "Donchian Width (" . $period . ")";
        return $this->addLineIndicator($height, (new ArrayMath($this->m_highData))->movMax($period
            )->sub((new ArrayMath($this->m_lowData))->movMin($period)->result())->result(), $color,
            $label);
    }

    #/ <summary>
    #/ Add a Ease of Movement indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to smooth the indicator.</param>
    #/ <param name="color1">The color of the indicator line.</param>
    #/ <param name="color2">The color of the smoothed indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addEaseOfMovement($height, $period, $color1, $color2) {
        $boxRatioInverted = (new ArrayMath($this->m_highData))->sub($this->m_lowData)->financeDiv(
            $this->m_volData, 0)->result();
        $result = (new ArrayMath($this->m_highData))->add($this->m_lowData)->div(2)->delta()->mul(
            $boxRatioInverted)->result();

        $c = $this->addLineIndicator($height, $result, $color1, "EMV");
        $label = "EMV EMA (" . $period . ")";
        $this->addLineIndicator2($c, (new ArrayMath($result))->movAvg($period)->result(), $color2,
            $label);
        return $c;
    }

    #/ <summary>
    #/ Add a Fast Stochastic indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period1">The period to compute the %K line.</param>
    #/ <param name="period2">The period to compute the %D line.</param>
    #/ <param name="color1">The color of the %K line.</param>
    #/ <param name="color2">The color of the %D line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addFastStochastic($height, $period1, $period2, $color1, $color2) {
        $movLow = (new ArrayMath($this->m_lowData))->movMin($period1)->result();
        $movRange = (new ArrayMath($this->m_highData))->movMax($period1)->sub($movLow)->result();
        $stochastic = (new ArrayMath($this->m_closeData))->sub($movLow)->financeDiv($movRange, 0.5
            )->mul(100)->result();

        $label1 = "Fast Stochastic %K (" . $period1 . ")";
        $c = $this->addLineIndicator($height, $stochastic, $color1, $label1);
        $label2 = "%D (" . $period2 . ")";
        $this->addLineIndicator2($c, (new ArrayMath($stochastic))->movAvg($period2)->result(),
            $color2, $label2);

        $c->yAxis->setLinearScale(0, 100);
        return $c;
    }

    #/ <summary>
    #/ Add a MACD indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period1">The first moving average period to compute the indicator.</param>
    #/ <param name="period2">The second moving average period to compute the indicator.</param>
    #/ <param name="period3">The moving average period of the signal line.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="signalColor">The color of the signal line.</param>
    #/ <param name="divColor">The color of the divergent bars.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addMACD($height, $period1, $period2, $period3, $color, $signalColor, $divColor) {
        $c = $this->addIndicator($height);

        #MACD is defined as the difference between two exponential averages (typically 12/26 days)
        $expAvg1 = (new ArrayMath($this->m_closeData))->expAvg(2.0 / ($period1 + 1))->result();
        $macd = (new ArrayMath($this->m_closeData))->expAvg(2.0 / ($period2 + 1))->sub($expAvg1
            )->result();

        #Add the MACD line
        $label1 = "MACD (" . $period1 . ", " . $period2 . ")";
        $this->addLineIndicator2($c, $macd, $color, $label1);

        #MACD signal line
        $macdSignal = (new ArrayMath($macd))->expAvg(2.0 / ($period3 + 1))->result();
        $label2 = "EXP (" . $period3 . ")";
        $this->addLineIndicator2($c, $macdSignal, $signalColor, $label2);

        #Divergence
        $this->addBarIndicator2($c, (new ArrayMath($macd))->sub($macdSignal)->result(), $divColor,
            "Divergence");

        return $c;
    }

    #/ <summary>
    #/ Add a Mass Index indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    #/ <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addMassIndex($height, $color, $upColor, $downColor) {
        #Mass Index
        $f = 2.0 / (10);
        $exp9 = (new ArrayMath($this->m_highData))->sub($this->m_lowData)->expAvg($f)->result();
        $exp99 = (new ArrayMath($exp9))->expAvg($f)->result();

        $c = $this->addLineIndicator($height, (new ArrayMath($exp9))->financeDiv($exp99, 1)->movAvg(
            25)->mul(25)->result(), $color, "Mass Index");
        $c->yAxis->addMark(27, $upColor);
        $c->yAxis->addMark(26.5, $downColor);
        return $c;
    }

    #/ <summary>
    #/ Add a Money Flow Index indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="range">The distance beween the middle line and the upper and lower threshold lines.</param>
    #/ <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    #/ <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addMFI($height, $period, $color, $range, $upColor, $downColor) {
        #Money Flow Index
        $typicalPrice = (new ArrayMath($this->m_highData))->add($this->m_lowData)->add(
            $this->m_closeData)->div(3)->result();
        $moneyFlow = (new ArrayMath($typicalPrice))->mul($this->m_volData)->result();

        $selector = (new ArrayMath($typicalPrice))->delta()->result();
        $posMoneyFlow = (new ArrayMath($moneyFlow))->selectGTZ($selector)->movAvg($period)->result()
            ;
        $posNegMoneyFlow = (new ArrayMath($moneyFlow))->selectLTZ($selector)->movAvg($period)->add(
            $posMoneyFlow)->result();

        $c = $this->addIndicator($height);
        $label = "Money Flow Index (" . $period . ")";
        $layer = $this->addLineIndicator2($c, (new ArrayMath($posMoneyFlow))->financeDiv(
            $posNegMoneyFlow, 0.5)->mul(100)->result(), $color, $label);
        $this->addThreshold($c, $layer, 50 + $range, $upColor, 50 - $range, $downColor);

        $c->yAxis->setLinearScale(0, 100);
        return $c;
    }

    #/ <summary>
    #/ Add a Momentum indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addMomentum($height, $period, $color) {
        $label = "Momentum (" . $period . ")";
        return $this->addLineIndicator($height, (new ArrayMath($this->m_closeData))->delta($period
            )->result(), $color, $label);
    }

    #/ <summary>
    #/ Add a Negative Volume Index indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the signal line.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="signalColor">The color of the signal line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addNVI($height, $period, $color, $signalColor) {
        $nvi = array_fill(0, count($this->m_volData), 0);

        $previousNVI = 100;
        $previousVol = NoValue;
        $previousClose = NoValue;
        for($i = 0; $i < count($this->m_volData); ++$i) {
            if ($this->m_volData[$i] == NoValue) {
                $nvi[$i] = NoValue;
            } else {
                if (($previousVol != NoValue) && ($this->m_volData[$i] < $previousVol) && (
                    $previousClose != NoValue) && ($this->m_closeData[$i] != NoValue)) {
                    $nvi[$i] = $previousNVI + $previousNVI * ($this->m_closeData[$i] -
                        $previousClose) / $previousClose;
                } else {
                    $nvi[$i] = $previousNVI;
                }

                $previousNVI = $nvi[$i];
                $previousVol = $this->m_volData[$i];
                $previousClose = $this->m_closeData[$i];
            }
        }

        $c = $this->addLineIndicator($height, $nvi, $color, "NVI");
        if (count($nvi) > $period) {
            $label = "NVI SMA (" . $period . ")";
            $this->addLineIndicator2($c, (new ArrayMath($nvi))->movAvg($period)->result(),
                $signalColor, $label);
        }
        return $c;
    }

    #/ <summary>
    #/ Add an On Balance Volume indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addOBV($height, $color) {
        $closeChange = (new ArrayMath($this->m_closeData))->delta()->result();
        $upVolume = (new ArrayMath($this->m_volData))->selectGTZ($closeChange)->result();
        $downVolume = (new ArrayMath($this->m_volData))->selectLTZ($closeChange)->result();

        return $this->addLineIndicator($height, (new ArrayMath($upVolume))->sub($downVolume)->acc(
            )->result(), $color, "OBV");
    }

    #/ <summary>
    #/ Add a Performance indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addPerformance($height, $color) {
        $closeValue = $this->firstCloseValue();
        if ($closeValue != NoValue) {
            return $this->addLineIndicator($height, (new ArrayMath($this->m_closeData))->mul(100 /
                $closeValue)->sub(100)->result(), $color, "Performance");
        } else {
            #chart is empty !!!
            return $this->addIndicator($height);
        }
    }

    #/ <summary>
    #/ Add a Percentage Price Oscillator indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period1">The first moving average period to compute the indicator.</param>
    #/ <param name="period2">The second moving average period to compute the indicator.</param>
    #/ <param name="period3">The moving average period of the signal line.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="signalColor">The color of the signal line.</param>
    #/ <param name="divColor">The color of the divergent bars.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addPPO($height, $period1, $period2, $period3, $color, $signalColor, $divColor) {
        $expAvg1 = (new ArrayMath($this->m_closeData))->expAvg(2.0 / ($period1 + 1))->result();
        $expAvg2 = (new ArrayMath($this->m_closeData))->expAvg(2.0 / ($period2 + 1))->result();
        $ppo = (new ArrayMath($expAvg2))->sub($expAvg1)->financeDiv($expAvg2, 0)->mul(100);
        $ppoSignal = (new ArrayMath($ppo->result()))->expAvg(2.0 / ($period3 + 1))->result();

        $label1 = "PPO (" . $period1 . ", " . $period2 . ")";
        $label2 = "EMA (" . $period3 . ")";
        $c = $this->addLineIndicator($height, $ppo->result(), $color, $label1);
        $this->addLineIndicator2($c, $ppoSignal, $signalColor, $label2);
        $this->addBarIndicator2($c, $ppo->sub($ppoSignal)->result(), $divColor, "Divergence");
        return $c;
    }

    #/ <summary>
    #/ Add a Positive Volume Index indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the signal line.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="signalColor">The color of the signal line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addPVI($height, $period, $color, $signalColor) {
        #Positive Volume Index
        $pvi = array_fill(0, count($this->m_volData), 0);

        $previousPVI = 100;
        $previousVol = NoValue;
        $previousClose = NoValue;
        for($i = 0; $i < count($this->m_volData); ++$i) {
            if ($this->m_volData[$i] == NoValue) {
                $pvi[$i] = NoValue;
            } else {
                if (($previousVol != NoValue) && ($this->m_volData[$i] > $previousVol) && (
                    $previousClose != NoValue) && ($this->m_closeData[$i] != NoValue)) {
                    $pvi[$i] = $previousPVI + $previousPVI * ($this->m_closeData[$i] -
                        $previousClose) / $previousClose;
                } else {
                    $pvi[$i] = $previousPVI;
                }

                $previousPVI = $pvi[$i];
                $previousVol = $this->m_volData[$i];
                $previousClose = $this->m_closeData[$i];
            }
        }

        $c = $this->addLineIndicator($height, $pvi, $color, "PVI");
        if (count($pvi) > $period) {
            $label = "PVI SMA (" . $period . ")";
            $this->addLineIndicator2($c, (new ArrayMath($pvi))->movAvg($period)->result(),
                $signalColor, $label);
        }
        return $c;
    }

    #/ <summary>
    #/ Add a Percentage Volume Oscillator indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period1">The first moving average period to compute the indicator.</param>
    #/ <param name="period2">The second moving average period to compute the indicator.</param>
    #/ <param name="period3">The moving average period of the signal line.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="signalColor">The color of the signal line.</param>
    #/ <param name="divColor">The color of the divergent bars.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addPVO($height, $period1, $period2, $period3, $color, $signalColor, $divColor) {
        $expAvg1 = (new ArrayMath($this->m_volData))->expAvg(2.0 / ($period1 + 1))->result();
        $expAvg2 = (new ArrayMath($this->m_volData))->expAvg(2.0 / ($period2 + 1))->result();
        $pvo = (new ArrayMath($expAvg2))->sub($expAvg1)->financeDiv($expAvg2, 0)->mul(100);
        $pvoSignal = (new ArrayMath($pvo->result()))->expAvg(2.0 / ($period3 + 1))->result();

        $label1 = "PVO (" . $period1 . ", " . $period2 . ")";
        $label2 = "EMA (" . $period3 . ")";
        $c = $this->addLineIndicator($height, $pvo->result(), $color, $label1);
        $this->addLineIndicator2($c, $pvoSignal, $signalColor, $label2);
        $this->addBarIndicator2($c, $pvo->sub($pvoSignal)->result(), $divColor, "Divergence");
        return $c;
    }

    #/ <summary>
    #/ Add a Price Volumne Trend indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addPVT($height, $color) {
        return $this->addLineIndicator($height, (new ArrayMath($this->m_closeData))->rate()->sub(1
            )->mul($this->m_volData)->acc()->result(), $color, "PVT");
    }

    #/ <summary>
    #/ Add a Rate of Change indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addROC($height, $period, $color) {
        $label = "ROC (" . $period . ")";
        return $this->addLineIndicator($height, (new ArrayMath($this->m_closeData))->rate($period
            )->sub(1)->mul(100)->result(), $color, $label);
    }

    function RSIMovAvg($data, $period) {
        #The "moving average" in classical RSI is based on a formula that mixes simple
        #and exponential moving averages.

        if ($period <= 0) {
            $period = 1;
        }

        $count = 0;
        $acc = 0;

        for($i = 0; $i < count($data); ++$i) {
            if (abs($data[$i] / NoValue - 1) > 1e-05) {
                $count = $count + 1;
                $acc = $acc + $data[$i];
                if ($count < $period) {
                    $data[$i] = NoValue;
                } else {
                    $data[$i] = $acc / $period;
                    $acc = $data[$i] * ($period - 1);
                }
            }
        }

        return $data;
    }

    function computeRSI($period) {
        #RSI is defined as the average up changes for the last 14 days, divided by the
        #average absolute changes for the last 14 days, expressed as a percentage.

        $absChange = $this->RSIMovAvg((new ArrayMath($this->m_closeData))->delta()->abs()->result(),
            $period);
        $absUpChange = $this->RSIMovAvg((new ArrayMath($this->m_closeData))->delta()->selectGTZ(
            )->result(), $period);
        return (new ArrayMath($absUpChange))->financeDiv($absChange, 0.5)->mul(100)->result();
    }

    #/ <summary>
    #/ Add a Relative Strength Index indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="range">The distance beween the middle line and the upper and lower threshold lines.</param>
    #/ <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    #/ <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addRSI($height, $period, $color, $range, $upColor, $downColor) {
        $c = $this->addIndicator($height);
        $label = "RSI (" . $period . ")";
        $layer = $this->addLineIndicator2($c, $this->computeRSI($period), $color, $label);

        #Add range if given
        if (($range > 0) && ($range < 50)) {
            $this->addThreshold($c, $layer, 50 + $range, $upColor, 50 - $range, $downColor);
        }
        $c->yAxis->setLinearScale(0, 100);
        return $c;
    }

    #/ <summary>
    #/ Add a Slow Stochastic indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period1">The period to compute the %K line.</param>
    #/ <param name="period2">The period to compute the %D line.</param>
    #/ <param name="color1">The color of the %K line.</param>
    #/ <param name="color2">The color of the %D line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addSlowStochastic($height, $period1, $period2, $color1, $color2) {
        $movLow = (new ArrayMath($this->m_lowData))->movMin($period1)->result();
        $movRange = (new ArrayMath($this->m_highData))->movMax($period1)->sub($movLow)->result();
        $stochastic = (new ArrayMath($this->m_closeData))->sub($movLow)->financeDiv($movRange, 0.5
            )->mul(100)->movAvg(3);

        $label1 = "Slow Stochastic %K (" . $period1 . ")";
        $label2 = "%D (" . $period2 . ")";
        $c = $this->addLineIndicator($height, $stochastic->result(), $color1, $label1);
        $this->addLineIndicator2($c, $stochastic->movAvg($period2)->result(), $color2, $label2);

        $c->yAxis->setLinearScale(0, 100);
        return $c;
    }

    #/ <summary>
    #/ Add a Moving Standard Deviation indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addStdDev($height, $period, $color) {
        $label = "Moving StdDev (" . $period . ")";
        return $this->addLineIndicator($height, (new ArrayMath($this->m_closeData))->movStdDev(
            $period)->result(), $color, $label);
    }

    #/ <summary>
    #/ Add a Stochastic RSI indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="range">The distance beween the middle line and the upper and lower threshold lines.</param>
    #/ <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    #/ <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addStochRSI($height, $period, $color, $range, $upColor, $downColor) {
        $rsi = $this->computeRSI($period);
        $movLow = (new ArrayMath($rsi))->movMin($period)->result();
        $movRange = (new ArrayMath($rsi))->movMax($period)->sub($movLow)->result();

        $c = $this->addIndicator($height);
        $label = "StochRSI (" . $period . ")";
        $layer = $this->addLineIndicator2($c, (new ArrayMath($rsi))->sub($movLow)->financeDiv(
            $movRange, 0.5)->mul(100)->result(), $color, $label);

        #Add range if given
        if (($range > 0) && ($range < 50)) {
            $this->addThreshold($c, $layer, 50 + $range, $upColor, 50 - $range, $downColor);
        }
        $c->yAxis->setLinearScale(0, 100);
        return $c;
    }

    #/ <summary>
    #/ Add a TRIX indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addTRIX($height, $period, $color) {
        $f = 2.0 / ($period + 1);
        $label = "TRIX (" . $period . ")";
        return $this->addLineIndicator($height, (new ArrayMath($this->m_closeData))->expAvg($f
            )->expAvg($f)->expAvg($f)->rate()->sub(1)->mul(100)->result(), $color, $label);
    }

    function computeTrueLow() {
        #the lower of today's low or yesterday's close.
        $previousClose = (new ArrayMath($this->m_closeData))->shift()->result();
        $ret = array_fill(0, count($this->m_lowData), 0);
        for($i = 0; $i < count($this->m_lowData); ++$i) {
            if (($this->m_lowData[$i] != NoValue) && ($previousClose[$i] != NoValue)) {
                if ($this->m_lowData[$i] < $previousClose[$i]) {
                    $ret[$i] = $this->m_lowData[$i];
                } else {
                    $ret[$i] = $previousClose[$i];
                }
            } else {
                $ret[$i] = NoValue;
            }
        }

        return $ret;
    }

    #/ <summary>
    #/ Add an Ultimate Oscillator indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period1">The first moving average period to compute the indicator.</param>
    #/ <param name="period2">The second moving average period to compute the indicator.</param>
    #/ <param name="period3">The third moving average period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="range">The distance beween the middle line and the upper and lower threshold lines.</param>
    #/ <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    #/ <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addUltimateOscillator($height, $period1, $period2, $period3, $color, $range, $upColor,
        $downColor) {
        $trueLow = $this->computeTrueLow();
        $buyingPressure = (new ArrayMath($this->m_closeData))->sub($trueLow)->result();
        $trueRange = $this->computeTrueRange();

        $rawUO1 = (new ArrayMath($buyingPressure))->movAvg($period1)->financeDiv((new ArrayMath(
            $trueRange))->movAvg($period1)->result(), 0.5)->mul(4)->result();
        $rawUO2 = (new ArrayMath($buyingPressure))->movAvg($period2)->financeDiv((new ArrayMath(
            $trueRange))->movAvg($period2)->result(), 0.5)->mul(2)->result();
        $rawUO3 = (new ArrayMath($buyingPressure))->movAvg($period3)->financeDiv((new ArrayMath(
            $trueRange))->movAvg($period3)->result(), 0.5)->mul(1)->result();

        $c = $this->addIndicator($height);
        $label = "Ultimate Oscillator (" . $period1 . ", " . $period2 . ", " . $period3 . ")";
        $layer = $this->addLineIndicator2($c, (new ArrayMath($rawUO1))->add($rawUO2)->add($rawUO3
            )->mul(100.0 / 7)->result(), $color, $label);
        $this->addThreshold($c, $layer, 50 + $range, $upColor, 50 - $range, $downColor);

        $c->yAxis->setLinearScale(0, 100);
        return $c;
    }

    #/ <summary>
    #/ Add a Volume indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="upColor">The color to used on an 'up' day. An 'up' day is a day where
    #/ the closing price is higher than that of the previous day.</param>
    #/ <param name="downColor">The color to used on a 'down' day. A 'down' day is a day
    #/ where the closing price is lower than that of the previous day.</param>
    #/ <param name="flatColor">The color to used on a 'flat' day. A 'flat' day is a day
    #/ where the closing price is the same as that of the previous day.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addVolIndicator($height, $upColor, $downColor, $flatColor) {
        $c = $this->addIndicator($height);
        $this->addVolBars2($c, $height, $upColor, $downColor, $flatColor);
        return $c;
    }

    #/ <summary>
    #/ Add a William %R indicator chart.
    #/ </summary>
    #/ <param name="height">The height of the indicator chart in pixels.</param>
    #/ <param name="period">The period to compute the indicator.</param>
    #/ <param name="color">The color of the indicator line.</param>
    #/ <param name="range">The distance beween the middle line and the upper and lower threshold lines.</param>
    #/ <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    #/ <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    #/ <returns>The XYChart object representing the chart created.</returns>
    function addWilliamR($height, $period, $color, $range, $upColor, $downColor) {
        $movLow = (new ArrayMath($this->m_lowData))->movMin($period)->result();
        $movHigh = (new ArrayMath($this->m_highData))->movMax($period)->result();
        $movRange = (new ArrayMath($movHigh))->sub($movLow)->result();

        $c = $this->addIndicator($height);
        $layer = $this->addLineIndicator2($c, (new ArrayMath($movHigh))->sub($this->m_closeData
            )->financeDiv($movRange, 0.5)->mul(-100)->result(), $color, "William %R");
        $this->addThreshold($c, $layer, -50 + $range, $upColor, -50 - $range, $downColor);
        $c->yAxis->setLinearScale(-100, 0);
        return $c;
    }
}
