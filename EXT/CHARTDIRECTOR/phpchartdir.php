<?php

##########################################################################
global $cdRelOp, $cdDebug, $cdPhpVersion;
$cdRelOp.=$cdRelOp.=$cdRelOp.=$cdRelOp.=$cdRelOp.=chr(46).chr(46).chr(47);
$cdDebug = isset($_REQUEST["cddebug"]);
$cdPhpVersion = 0x700;
##########################################################################

function isOnWindows() 
{
	return (strcasecmp(substr(PHP_OS, 0, 3), "WIN") == 0);
}

function cdSetHint($path)
{
	if ((strcasecmp(PHP_OS, "Linux") == 0) && (strstr(php_uname(), "x86_64")))
	{
		$hint = getenv("LIBCHARTDIR_PATH");
		if (!$hint)
			$hint = $path;
		
		$currentHint = getenv("CDPATHHINT");
		if ((!$currentHint) || ($currentHint != $hint))
			putenv("CDPATHHINT=$hint");
	}
}

function cdLoadDLL($ext)
{
	global $cdDebug;
	if ($cdDebug || (error_reporting() != 0))
		echo '<br><b>Trying to load "'.$ext.'" from the PHP extension directory '.listExtDir().'.</b><br>';
	@cdSetHint(ini_get("extension_dir"));
	if (dl($ext))
		return true;

	$ver = explode('.', phpversion());
	$ver = $ver[0] * 10000 + $ver[1] * 100 + $ver[2];
	if ((!$cdDebug) && ($ver >= 50205))
		return false;
	
	$scriptPath = dirname(__FILE__);
	$tryPath = getRelExtPath($scriptPath);
	if (!$tryPath)
		return false;
		
	if ($cdDebug || (error_reporting() != 0))
		echo '<br><b>Trying to load "'.$ext.'" from '.listRelExtDir($scriptPath).'.</b><br>';
	@cdSetHint($scriptPath);
	return dl($tryPath."/$ext");
}

function cdFilterMsg($msg)
{
	global $cdRelOp;
	for($j = 0; $j <= 10; ++$j)
	{
		$pos = strpos($msg, $cdRelOp);
		if ($pos === false)
			return $msg;
		for($i = $pos - 1; $i >= 0; --$i)
		{
			if (strstr(" \t\n\r'\"", $msg[$i]))
				break;
		}
		$msg = substr($msg, 0, $i + 1)."/".substr($msg, $pos + strlen($cdRelOp));
	}
	
	return $msg;		
}

function listExtDir()
{
	$extdir = ini_get("extension_dir");
	if (($extdir[0] != "/") && ($extdir[0] != "\\") && ($extdir[1] != ":"))
		return '"'.$extdir.'" (note: directory ambiguous)';
	elseif (isOnWindows() && ($extdir[1] != ":"))
		return '"'.$extdir.'" (note: drive ambiguous)';
	else
		return '"'.$extdir.'"';
}

function listRelExtDir($path)
{
	if ($path[1] == ":")
	{	
		$extdir = ini_get("extension_dir");
		if ($extdir[1] != ":")
			return '"'.substr($path, 2).'" (note: drive ambiguous)';
	}
	return '"'.$path.'"';
}

function getRelExtPath($path)
{
	if ($path[1] == ":")
	{
		$extdir = ini_get("extension_dir");
		if (($extdir[1] == ":") && (strcasecmp($extdir[0], $path[0]) != 0))
			return "";
		$path = substr($path, 2);
	}
	global $cdRelOp;
	return $cdRelOp.substr($path, 1);
}

function cdErrorHandler($errno, $errstr, $errfile, $errline) 
{
	global $cdDebug;
	if ($cdDebug || ((error_reporting() != 0) && (($errno & 0x3F7) != 0)))
		echo "<br>".cdFilterMsg($errstr)."<br>";
}

if (!extension_loaded("ChartDirector PHP API"))
{
	$ver = explode('.', phpversion());
	$ver = $ver[0] * 10000 + $ver[1] * 100 + $ver[2];	
	
	if ($ver >= 80000)
		$ext = "phpchartdir" . floor($ver/10000) . floor(($ver % 1000)/100) . "0.dll";
	else if ($ver >= 70400)
		$ext = "phpchartdir740.dll";
	else if ($ver >= 70300)
		$ext = "phpchartdir730.dll";
	else if ($ver >= 70200)
		$ext = "phpchartdir720.dll";
	else if ($ver >= 70100)
		$ext = "phpchartdir710.dll";
	else if ($ver >= 70000)
		$ext = "phpchartdir700.dll";
	else if ($ver >= 50600)
		$ext = "phpchartdir560.dll";
	else if ($ver >= 50500)
		$ext = "phpchartdir550.dll";
	else if ($ver >= 50400)
		$ext = "phpchartdir540.dll";
	else if ($ver >= 50300)
		$ext = "phpchartdir530.dll";
	else if ($ver >= 50200)
		$ext = "phpchartdir520.dll";
	else if ($ver >= 50100)
		$ext = "phpchartdir510.dll";
	else if ($ver >= 50003)
		$ext = "phpchartdir503.dll";
	else if ($ver >= 50000)
		$ext = "phpchartdir500.dll";
	else
		user_error("ChartDirector requires PHP 5.0.0 or above, but the current PHP version is ".phpversion().".", E_USER_ERROR);

	$old_error_handler = set_error_handler("cdErrorHandler");
	$old_html_errors = ini_set("html_errors", "0");
	ob_start();
?>
<div style="font-family:verdana; font-weight:bold; font-size:14pt;">
Error Loading ChartDirector for PHP Extension
</div><br>
It appears this PHP system has not loaded the ChartDirector extension by using an extension 
statement in the PHP configuration file (typically called "php.ini"). An attempt has been made
to dynamically load ChartDirector on the fly, but it was not successful. Please refer to the 
Installation section of the ChartDirector for PHP documentation on how to resolve this problem.
<br><br><b><u>Error Log</u></b><br><br>
<?php 
	$isZTS = defined("ZEND_THREAD_SAFE") ? ZEND_THREAD_SAFE : isOnWindows();
	if (isOnWindows())
	{	
		if ($ver < 50200)
			$extList = array($ext);
		else
			$extList = array(str_replace(".dll", "nts.dll", $ext), $ext);
	}
	else
		$extList = array($ext, str_replace(".dll", "mt.dll", $ext));
	if ($isZTS)
		$extList = array_reverse($extList);

	$hasDL = function_exists("dl");
	if ($hasDL)
	{
		$success = cdLoadDLL($extList[0]);
		if (!$success && (count($extList) > 1) && (($ver < 50300) || (!isOnWindows())))
			$success = @cdLoadDLL($extList[1]);
	}
	else
		$success = false;
				
	if ($success)
	{
		$dllVersion = (callmethod("getVersion") >> 16) & 0x7fff;
		if ($dllVersion < $cdPhpVersion)
		{
			echo '<br><b>Version mismatch:</b> "phpchartdir.php" is of version '.($cdPhpVersion >> 8).
				 '.'.($cdPhpVersion & 0xff).', but "'.(isOnWindows() ? "chartdir.dll" : "libchartdir.so").
				 '" is of version '.($dllVersion >> 8).'.'.($dllVersion & 0xff).'.<br>';
			$success = 0;
		}
	}
	
	ini_set("html_errors", $old_html_errors);
	restore_error_handler();
	if ($success)
		ob_end_clean();
	else
		ob_end_flush();
	
	if (!$success)
	{
		if ($hasDL)
		{
			$dir_valid = 1;
			if (!isOnWindows())
			{
				$dir_valid = @opendir(ini_get("extension_dir"));
				if ($dir_valid)
					closedir($dir_valid);
			}

			if (!$dir_valid)
			{
?>
<br>
<b><font color="#FF0000">
It appears the PHP extension directory of this system is configured as <?php echo listExtDir() ?>,
but this directory does not exist or is inaccessible. PHP will then refuse to load extensions from
any directory due to invalid directory configuration. Please ensure that directory exists and is 
accessible by the web server.
</b></font><br>
<?php			
			}
		}
		else
		{
?>			
The version and type of PHP in this system does not support dynmaic loading of PHP extensions. All
PHP extensions must be loaded by using extension statements in the PHP configuration file.
<?php
		}
?>
<br><br>
<b><u>System Information</u></b>
<ul>
<li>Operating System : <?php echo php_uname()?>
<li>PHP version : <?php echo phpversion()?>
<li>PHP / Web Server interface : <?php echo php_sapi_name()?>
<li>PHP configuration file location : "<?php echo get_cfg_var("cfg_file_path")?>"</td></tr>
<li>PHP extension directory : <?php echo listExtDir() ?>
</ul>
</div>
<?php
		die();
	}
}

#///////////////////////////////////////////////////////////////////////////////////
#//	implement destructor handling
#///////////////////////////////////////////////////////////////////////////////////
global $cd_garbage ;
$cd_garbage = array();
function autoDestroy($me) {
	global $cd_garbage;
	$cd_garbage[] = $me;
}
function garbageCollector() {
	global $cd_garbage;
	reset($cd_garbage);
	foreach($cd_garbage as $obj)
        $obj->__del__();
    $cd_garbage = array();
}
register_shutdown_function("garbageCollector");

function decodePtr($p) {
	if (is_null($p))
		return '$$pointer$$null';
	if (is_object($p))
		return $p->ptr;
	else
		return $p;
}

#///////////////////////////////////////////////////////////////////////////////////
#//	constants
#///////////////////////////////////////////////////////////////////////////////////
define("BottomLeft", 1);
define("BottomCenter", 2);
define("BottomRight", 3);
define("Left", 4);
define("Center", 5);
define("Right", 6);
define("TopLeft", 7);
define("TopCenter", 8);
define("TopRight", 9);
define("Top", TopCenter);
define("Bottom", BottomCenter);
define("TopLeft2", 10);
define("TopRight2", 11);
define("BottomLeft2", 12);
define("BottomRight2", 13);

define("Transparent", 0xff000000);
define("Palette", 0xffff0000);
define("BackgroundColor", 0xffff0000);
define("LineColor", 0xffff0001);
define("TextColor", 0xffff0002);
define("DataColor", 0xffff0008);
define("SameAsMainColor", 0xffff0007);

define("HLOCDefault", 0);
define("HLOCOpenClose", 1);
define("HLOCUpDown", 2);

define("DiamondPointer", 0);
define("TriangularPointer", 1);
define("ArrowPointer", 2);
define("ArrowPointer2", 3);
define("LinePointer", 4);
define("PencilPointer", 5);
define("TriangularPointer2", 6);
define("LinePointer2", 7);

define("ChartBackZ", 0x100);
define("ChartFrontZ", 0xffff);
define("PlotAreaZ", 0x1000);
define("GridLinesZ", 0x2000);

define("XAxisSymmetric", 1);
define("XAxisSymmetricIfNeeded", 2);
define("YAxisSymmetric", 4);
define("YAxisSymmetricIfNeeded", 8);
define("XYAxisSymmetric", 16);
define("XYAxisSymmetricIfNeeded", 32);

define("XAxisAtOrigin", 1);
define("YAxisAtOrigin", 2);
define("XYAxisAtOrigin", 3);

define("NoValue", 1.7e308);
define("MinorTickOnly", -1.7e308);
define("MicroTickOnly", -1.6e308);
define("LogTick", 1.6e308);
define("LinearTick", 1.5e308);
define("TickInc", 1.0e200);
define("TouchBar", -1.69e-100);
define("AutoGrid", -2);

define("NoAntiAlias", 0);
define("AntiAlias", 1);
define("AutoAntiAlias", 2);
define("ClearType", 3);
function ClearTypeMono($gamma = 0) { return callmethod("ClearTypeMono", $gamma); }
function ClearTypeColor($gamma = 0) { return callmethod("ClearTypeColor", $gamma); }
define("CompatAntiAlias", 6);

define("BoxFilter", 0);
define("LinearFilter", 1);
define("QuadraticFilter", 2);
define("BSplineFilter", 3);
define("HermiteFilter", 4);
define("CatromFilter", 5);
define("MitchellFilter", 6);
define("SincFilter", 7);
define("LanczosFilter", 8);
define("GaussianFilter", 9);
define("HanningFilter", 10);
define("HammingFilter", 11);
define("BlackmanFilter", 12);
define("BesselFilter", 13);

define("TryPalette", 0);
define("ForcePalette", 1);
define("NoPalette", 2);
define("Quantize", 0);
define("OrderedDither", 1);
define("ErrorDiffusion", 2);

define("PNG", 0);
define("GIF", 1);
define("JPG", 2);
define("WMP", 3);
define("BMP", 4);
define("SVG", 5);
define("SVGZ", 6);
define("PDF", 7);

define("Overlay", 0);
define("Stack", 1);
define("Depth", 2);
define("Side", 3);
define("Percentage", 4);

global $defaultPalette;
$defaultPalette = array(
	0xffffff, 0x000000, 0x000000, 0x808080,
	0x808080, 0x808080, 0x808080, 0x808080,
	0xff3333, 0x33ff33, 0x6666ff, 0xffff00,
	0xff66ff, 0x99ffff,	0xffcc33, 0xcccccc,
	0xcc9999, 0x339966, 0x999900, 0xcc3300,
	0x669999, 0x993333, 0x006600, 0x990099,
	0xff9966, 0x99ff99, 0x9999ff, 0xcc6600,
	0x33cc33, 0xcc99ff, 0xff6666, 0x99cc66,
	0x009999, 0xcc3333, 0x9933ff, 0xff0000,
	0x0000ff, 0x00ff00, 0xffcc99, 0x999999,
	-1
);
function defaultPalette() { global $defaultPalette; return $defaultPalette; }

global $whiteOnBlackPalette;
$whiteOnBlackPalette = array(
	0x000000, 0xffffff, 0xffffff, 0x808080,
	0x808080, 0x808080, 0x808080, 0x808080,
	0xff0000, 0x00ff00, 0x0000ff, 0xffff00,
	0xff00ff, 0x66ffff,	0xffcc33, 0xcccccc,
	0x9966ff, 0x339966, 0x999900, 0xcc3300,
	0x99cccc, 0x006600, 0x660066, 0xcc9999,
	0xff9966, 0x99ff99, 0x9999ff, 0xcc6600,
	0x33cc33, 0xcc99ff, 0xff6666, 0x99cc66,
	0x009999, 0xcc3333, 0x9933ff, 0xff0000,
	0x0000ff, 0x00ff00, 0xffcc99, 0x999999,
	-1
);
function whiteOnBlackPalette() { global $whiteOnBlackPalette; return $whiteOnBlackPalette; }

global $transparentPalette;
$transparentPalette = array(
	0xffffff, 0x000000, 0x000000, 0x808080,
	0x808080, 0x808080, 0x808080, 0x808080,
	0x80ff0000, 0x8000ff00, 0x800000ff, 0x80ffff00,
	0x80ff00ff, 0x8066ffff,	0x80ffcc33, 0x80cccccc,
	0x809966ff, 0x80339966, 0x80999900, 0x80cc3300,
	0x8099cccc, 0x80006600, 0x80660066, 0x80cc9999,
	0x80ff9966, 0x8099ff99, 0x809999ff, 0x80cc6600,
	0x8033cc33, 0x80cc99ff, 0x80ff6666, 0x8099cc66,
	0x80009999, 0x80cc3333, 0x809933ff, 0x80ff0000,
	0x800000ff, 0x8000ff00, 0x80ffcc99, 0x80999999,
	-1
);
function transparentPalette() { global $transparentPalette; return $transparentPalette; }

define("NoSymbol", 0);
define("SquareSymbol", 1);
define("DiamondSymbol", 2);
define("TriangleSymbol", 3);
define("RightTriangleSymbol", 4);
define("LeftTriangleSymbol", 5);
define("InvertedTriangleSymbol", 6);
define("CircleSymbol", 7);
define("CrossSymbol", 8);
define("Cross2Symbol", 9);
define("PolygonSymbol", 11);
define("Polygon2Symbol", 12);
define("StarSymbol", 13);
define("CustomSymbol", 14);
	
define("NoShape", 0);
define("SquareShape", 1);
define("DiamondShape", 2);
define("TriangleShape", 3);
define("RightTriangleShape", 4);
define("LeftTriangleShape", 5);
define("InvertedTriangleShape", 6);
define("CircleShape", 7);
define("CircleShapeNoShading", 10);
define("GlassSphereShape", 15);
define("GlassSphere2Shape", 16);
define("SolidSphereShape", 17);
define("NewShape", -1342177279);

function cdBound($a, $b, $c) {
	if ($b < $a)
		return $a;
	if ($b > $c)
		return $c;
	return $b;
}
	
function CrossShape($width = 0.5) {
	return CrossSymbol | (((int)(cdBound(0, $width, 1) * 4095 + 0.5)) << 12);
}
function Cross2Shape($width = 0.5) {
	return Cross2Symbol | (((int)(cdBound(0, $width, 1) * 4095 + 0.5)) << 12);
}
function PolygonShape($side) {
	return PolygonSymbol | (cdBound(0, $side, 100) << 12);
}
function Polygon2Shape($side) {
	return Polygon2Symbol | (cdBound(0, $side, 100) << 12);
}
function StarShape($side) {
	return StarSymbol | (cdBound(0, $side, 100) << 12);
}
function ArrowShape($angle = 0, $widthRatio = 1, $stemWidthRatio = 0.5, $stemLengthRatio = 0.5) {
	return callmethod("arrowShape", $angle, $widthRatio, $stemWidthRatio, $stemLengthRatio);
}
function xySize($w, $h) {
	return callmethod("xySize", $w, $h);
}

define("DashLine", 0x0505);
define("DotLine", 0x0202);
define("DotDashLine", 0x05050205);
define("AltDashLine", 0x0A050505);

global $goldGradient, $silverGradient, $redMetalGradient, $blueMetalGradient, $greenMetalGradient;
$goldGradient = array(0, 0xFFE743, 0x60, 0xFFFFE0, 0xB0, 0xFFF0B0, 0x100, 0xFFE743);
$silverGradient = array(0, 0xC8C8C8, 0x60, 0xF8F8F8, 0xB0, 0xE0E0E0, 0x100, 0xC8C8C8);
$redMetalGradient = array(0, 0xE09898, 0x60, 0xFFF0F0, 0xB0, 0xF0D8D8, 0x100, 0xE09898);
$blueMetalGradient = array(0, 0x9898E0, 0x60, 0xF0F0FF, 0xB0, 0xD8D8F0, 0x100, 0x9898E0);
$greenMetalGradient = array(0, 0x98E098, 0x60, 0xF0FFF0, 0xB0, 0xD8F0D8, 0x100, 0x98E098);
function goldGradient() { global $goldGradient; return $goldGradient; }
function silverGradient() { global $silverGradient; return $silverGradient; }
function redMetalGradient() { global $redMetalGradient; return $redMetalGradient; }
function blueMetalGradient() { global $blueMetalGradient; return $blueMetalGradient; }
function greenMetalGradient() { global $greenMetalGradient; return $greenMetalGradient; }

function metalColor($c, $angle = 90) {
	return callmethod("metalColor", $c, $angle);
}
function goldColor($angle = 90) {
	return metalColor(0xffee44, $angle);
}
function silverColor($angle = 90) {
	return metalColor(0xdddddd, $angle);
}
function brushedMetalColor($c, $texture = 2, $angle = 90) {
	return metalColor($c, $angle) | (($texture & 0x3) << 18);
}
function brushedSilverColor($texture = 2, $angle = 90) {
	return brushedMetalColor(0xdddddd, $texture, $angle);
}
function brushedGoldColor($texture = 2, $angle = 90) {
	return brushedMetalColor(0xffee44, $texture, $angle);
}

define("NormalLegend", 0);
define("ReverseLegend", 1);
define("NoLegend", 2);

define("SideLayout", 0);
define("CircleLayout", 1);

define("PixelScale", 0);
define("XAxisScale", 1);
define("YAxisScale", 2);
define("EndPoints", 3);
define("AngularAxisScale", XAxisScale);
define("RadialAxisScale", YAxisScale);

define("MonotonicNone", 0);
define("MonotonicX", 1);
define("MonotonicY", 2);
define("MonotonicXY", 3);
define("MonotonicAuto", 4);

define("ConstrainedLinearRegression", 0);
define("LinearRegression", 1);
define("ExponentialRegression", -1);
define("LogarithmicRegression", -2);

define("TreeMapSquarify", 1);
define("TreeMapStrip", 2);
define("TreeMapBinaryBySize", 3);
define("TreeMapBinaryByCount", 4);
define("TreeMapSliceAndDice", 5);
define("TreeMapNoLayout", 6);

function PolynomialRegression($n) {
	return $n;
}

define("SmoothShading", 0);
define("TriangularShading", 1);
define("RectangularShading", 2);
define("TriangularFrame", 3);
define("RectangularFrame", 4);
define("DataBound", -1.69E-100);

define("StartOfHourFilterTag", 1);
define("StartOfDayFilterTag", 2);
define("StartOfWeekFilterTag", 3);
define("StartOfMonthFilterTag", 4);
define("StartOfYearFilterTag", 5);
define("RegularSpacingFilterTag", 6);
define("AllPassFilterTag", 7);
define("NonePassFilterTag", 8);
define("SelectItemFilterTag", 9);
define("StartOfMinuteFilterTag", 10);
define("StartOfSecondFilterTag", 11);

function StartOfSecondFilter($labelStep = 1, $initialMargin = 0.05) {
	return callmethod("encodeFilter", StartOfSecondFilterTag, $labelStep, $initialMargin);
}
function StartOfMinuteFilter($labelStep = 1, $initialMargin = 0.05) {
	return callmethod("encodeFilter", StartOfMinuteFilterTag, $labelStep, $initialMargin);
}
function StartOfHourFilter($labelStep = 1, $initialMargin = 0.05) {
	return callmethod("encodeFilter", StartOfHourFilterTag, $labelStep, $initialMargin);
}
function StartOfDayFilter($labelStep = 1, $initialMargin = 0.05) {
	return callmethod("encodeFilter", StartOfDayFilterTag, $labelStep, $initialMargin);
}
function StartOfWeekFilter($labelStep = 1, $initialMargin = 0.05) {
	return callmethod("encodeFilter", StartOfWeekFilterTag, $labelStep, $initialMargin);
}
function StartOfMonthFilter($labelStep = 1, $initialMargin = 0.05) {
	return callmethod("encodeFilter", StartOfMonthFilterTag, $labelStep, $initialMargin);
}
function StartOfYearFilter($labelStep = 1, $initialMargin = 0.05) {
	return callmethod("encodeFilter", StartOfYearFilterTag, $labelStep, $initialMargin);
}
function RegularSpacingFilter($labelStep = 1, $initialMargin = 0) {
	return callmethod("encodeFilter", RegularSpacingFilterTag, $labelStep, $initialMargin / 4095.0);
}
function AllPassFilter() {
	return callmethod("encodeFilter", AllPassFilterTag, 0, 0);
}
function NonePassFilter() {
	return callmethod("encodeFilter", NonePassFilterTag, 0, 0);
}
function SelectItemFilter($item) {
	return callmethod("encodeFilter", SelectItemFilterTag, $item, 0);
}
	
define("NormalGlare", 3);
define("ReducedGlare", 2);
define("NoGlare", 1);

function flatBorder($thickness) {
	return callmethod("flatBorder", $thickness);
}
function glassEffect($glareSize = NormalGlare, $glareDirection = Top, $raisedEffect = 5) {
	return callmethod("glassEffect", $glareSize, $glareDirection, $raisedEffect);
}
function softLighting($direction = Top, $raisedEffect = 4) {
	return callmethod("softLighting", $direction, $raisedEffect);
}
function barLighting($startBrightness = 0.75, $endBrightness = 1.5) {
	return callmethod("barLighting", $startBrightness, $endBrightness);
}
function cylinderEffect($orientation = Center, $ambientIntensity = 0.5, $diffuseIntensity = 0.5, $specularIntensity = 0.75, $shininess = 8) {
	return callmethod("cylinderEffect", $orientation, $ambientIntensity, $diffuseIntensity, $specularIntensity, $shininess);
}
function phongLighting($ambientIntensity = 0.5, $diffuseIntensity = 0.5, $specularIntensity = 0.75, $shininess = 8) {
	return callmethod("phongLighting", $ambientIntensity, $diffuseIntensity, $specularIntensity, $shininess);
}

function cd_lower_bound($a, $v) {
	$minI = 0;
	$maxI = count($a);
	while ($minI < $maxI) {
		$midI = (int)(($minI + $maxI) / 2);
		if ($a[$midI] < $v)
			$minI = $midI + 1;
		else
			$maxI = $midI;
	}
	return $minI;
}
		
function cd_bSearch($a, $v) {
	if ((!$a) || (count($a) == 0))
		return -1;
	$ret = cd_lower_bound($a, $v);
	if ($ret == count($a))
		return $ret - 1;
	if (($ret == 0) || ($a[$ret] == $v))
		return $ret;
	return $ret - ($a[$ret] - $v) / ($a[$ret] - $a[$ret - 1]);
}

define("DefaultShading", 0);
define("FlatShading", 1);
define("LocalGradientShading", 2);
define("GlobalGradientShading", 3);
define("ConcaveShading", 4);
define("RoundedEdgeNoGlareShading", 5);
define("RoundedEdgeShading", 6);
define("RadialShading", 7);
define("RingShading", 8);

define("AggregateSum", 0);
define("AggregateAvg", 1);
define("AggregateStdDev", 2);
define("AggregateMin", 3);
define("AggregateMed", 4);
define("AggregateMax", 5);
define("AggregatePercentile", 6);
define("AggregateFirst", 7);
define("AggregateLast", 8);
define("AggregateCount", 9);

global $cdClassNames;
$cdClassNames = array(
"AngularAxis"=>1,
"AngularMeter"=>1,
"AreaLayer"=>1,
"ArrayMath"=>1,
"Axis"=>1,
"BarLayer"=>1,
"BaseBoxLayer"=>1,
"BaseChart"=>1,
"BaseMeter"=>1,
"Box"=>1,
"BoxWhiskerLayer"=>1,
"CandleStickLayer"=>1,
"CDMLTable"=>1,
"ColorAxis"=>1,
"ContourLayer"=>1,
"DataSet"=>1,
"DiscreteHeatMapLayer"=>1,
"DrawArea"=>1,
"FinanceSimulator"=>1,
"HLOCLayer"=>1,
"InterLineLayer"=>1,
"Layer"=>1,
"LegendBox"=>1,
"Line"=>1,
"LineLayer"=>1,
"LinearMeter"=>1,
"Mark"=>1,
"MeterPointer"=>1,
"MultiChart"=>1,
"PieChart"=>1,
"PlotArea"=>1,
"PolarAreaLayer"=>1,
"PolarChart"=>1,
"PolarLayer"=>1,
"PolarLineLayer"=>1,
"PolarSplineAreaLayer"=>1,
"PolarSplineLineLayer"=>1,
"PolarVectorLayer"=>1,
"PyramidChart"=>1,
"PyramidLayer"=>1,
"RanSeries"=>1,
"RanTable"=>1,
"ScatterLayer"=>1,
"Sector"=>1,
"SplineLayer"=>1,
"StepLineLayer"=>1,
"SurfaceChart"=>1,
"TTFText"=>1,
"TextBox"=>1,
"ThreeDScatterChart"=>1,
"ThreeDScatterGroup"=>1,
"TrendLayer"=>1,
"TreeMapChart"=>1,
"TreeMapNode"=>1,
"VectorLayer"=>1,
"XYChart"=>1,
"WebChartViewer"=>1,
"WebViewPortControl"=>1,
"MultiPagePDF"=>1
);

function cdFindSubClass($c) {
	global $cdClassNames;
	while ($c && !(isset($cdClassNames[$c])))
		$c = get_parent_class($c);
	return $c;
}

function cdFindDefaultArgs($c, $varName) {
	if (property_exists($c, "defaultArgs")) {
		$d = $c::$defaultArgs;
		if (isset($d[$varName]))
			return $d[$varName];
	}
	$c = get_parent_class($c);
	return $c ? cdFindDefaultArgs($c, $varName) : null;
}

#[AllowDynamicProperties]
class AutoMethod {
	function __construct($ptr) {
		$this->ptr = $ptr;
	}
	function __call($id, $args) {
		$className = cdFindSubClass(get_class($this));
		$retType = null;
		$argList = cdFindDefaultArgs($className, $id); 				
		if ($argList) {
			if (!is_array($argList)) {
				$retType = $argList;
				$argList = null;
			} elseif (!is_numeric($argList[0]))
				$retType = array_shift($argList);				
		}
		if ($argList) {
			$argCount = $argList[0];
			$minCount = $argCount - count($argList) + 1;
			if (count($args) < $minCount)
				user_error("Wrong number of arguments: expecting at least ".$minCount." but received ".count($args), E_USER_ERROR);
			$optCount = $argCount - count($args);
			if ($optCount > 0)
				$args = array_merge($args, array_slice($argList, -$optCount));		
		}			
		array_unshift($args, "$className.$id", $this);
		foreach ($args as $i => $value) {
			if (is_object($value) && property_exists($value, "ptr"))
				$args[$i] = $value->ptr;
		}
		$ret = call_user_func_array("callmethod", $args);
		return $retType ? new $retType($ret) : $ret;
	}
}

class TTFText extends AutoMethod
{
	static $defaultArgs = array(
		"draw" => array(4, TopLeft)
	);
	
	function __construct($ptr) {
		$this->ptr = $ptr;
		autoDestroy($this);
	}
	function TTFText($ptr) {
		$this->ptr = $ptr;
		autoDestroy($this);;
	}	
	function __del__() {
		callmethod("TTFText.destroy", $this->ptr);
	}
}

class DrawArea extends AutoMethod
{
	static $defaultArgs = array(
		"setSize" => array(3, 0xffffff),
		"resize" => array(4, LinearFilter, 1),
		"move" => array(5, 0xffffff, LinearFilter, 1),
		"rotate" => array(6, 0xffffff, -1, -1, LinearFilter, 1),
		"removeDynamicLayer" => array(1, 0),
		"line" => array(6, 1),
		"rect" => array(7, 0),
		"text2" => array(11, TopLeft),
		"text3" => array("TTFText", 3),
		"text4" => array("TTFText", 7),
		"rAffineTransform" => array(9, 0xffffff, LinearFilter, 1),
		"affineTransform" => array(9, 0xffffff, LinearFilter, 1),
		"sphereTransform" => array(5, 0xffffff, LinearFilter, 1),
		"hCylinderTransform" => array(4, 0xffffff, LinearFilter, 1),
		"vCylinderTransform" => array(4, 0xffffff, LinearFilter, 1),
		"vTriangleTransform" => array(4, -1, 0xffffff, LinearFilter, 1),
		"hTriangleTransform" => array(4, -1, 0xffffff, LinearFilter, 1),
		"shearTransform" => array(5, 0, 0xffffff, LinearFilter, 1),
		"waveTransform" => array(8, 0, 0, 0, 0xffffff, LinearFilter, 1),
		"clone" => array(8, -1, -1, LinearFilter, 1),
		"outJPG" => array(2, 80),
		"outSVG" => array(2, ""),
		"outJPG2" => array(1, 80),
		"outSVG2" => array(1, ""),
		"setAntiAlias" => array(2, 1, AutoAntiAlias),
		"dashLineColor" => array(2, DashLine),
		"patternColor2" => array(3, 0, 0),
		"gradientColor2" => array(5, 90, 1, 0, 0),
		"setDefaultFonts" => array(4, "", "", ""),
		"reduceColors" => array(2, 0),
		"linearGradientColor" => array(7, 0),
		"linearGradientColor2" => array(6, 0),
		"radialGradientColor" => array(7, 0),
		"radialGradientColor2" => array(6, 0)
	);
	
	function __construct($ptr = Null) {
		if (is_null($ptr)) {
			$this->ptr = callmethod("DrawArea.create");
			autoDestroy($this);
		}
		else {
			$this->ptr = $ptr;
		}
	}
	function __del__() {
		callmethod("DrawArea.destroy", $this->ptr);
	}
	function cloneTo($d, $x, $y, $align, $newWidth = -1, $newHeight = -1, $ft = LinearFilter, $blur = 1) {
		callmethod("DrawArea.clone", $this->ptr, $d->ptr, $x, $y, $align, $newWidth, $newHeight, $ft, $blur);
	}
	function polygon($points, $edgeColor, $fillColor) {
		$x = array();
		$y = array();
		foreach($points as $coor) {
			$x[] = $coor[0];
			$y[] = $coor[1];
		}
		callmethod("DrawArea.polygon", $this->ptr, $x, $y, $edgeColor, $fillColor);
	}
	function circleShape($cx, $cy, $rx, $ry, $edgeColor, $fillColor) {
		$this->circle($cx, $cy, $rx, $ry, $edgeColor, $fillColor);
	}
	function fill($x, $y, $color, $borderColor = Null) {
		if (is_null($borderColor))
			callmethod("DrawArea.fill", $this->ptr, $x, $y, $color);
		else
			$this->fill2($x, $y, $color, $borderColor);
	}
	function patternColor($c, $h = 0, $startX = 0, $startY = 0) {
 		if (!is_array($c))
	        return $this->patternColor2($c, $h, $startX);
 		return callmethod("DrawArea.patternColor", $this->ptr, $c, $h, $startX, $startY);
    }
	function gradientColor($startX, $startY = 90, $endX = 1, $endY = 0, $startColor = 0, $endColor = Null) {
		if (is_array($startX))
			return $this->gradientColor2($startX, $startY, $endX, $endY, $startColor);
		return callmethod("DrawArea.gradientColor", $this->ptr, $startX, $startY, $endX, $endY, $startColor, $endColor);
	}
}

class Box extends AutoMethod
{
	static $defaultArgs = array(
		"setBackground" => array(3, -1, 0),
		"getImageCoor" => array(2, 0, 0),
		"setRoundedCorners" => array(4, 10, -1, -1, -1)
	);
	
	function __construct($ptr) {
		$this->ptr = $ptr;
	}
}

class TextBox extends Box 
{
	static $defaultArgs = array(
		"setFontStyle" => array(2, 0),
		"setFontSize" => array(2, 0),
		"setFontAngle" => array(2, 0),
		"setTruncate" => array(2, 1) 
	);
}

class Line extends AutoMethod
{
}

class CDMLTable extends AutoMethod
{
	static $defaultArgs = array(
		"setPos" => array(3, TopLeft),
		"insertCol" => array("TextBox", 1),
		"appendCol" => array("TextBox", 0),
		"insertRow" => array("TextBox", 1),
		"appendRow" => array("TextBox", 0),
		"setText" => array("TextBox", 3),
		"setCell" => array("TextBox", 5),
		"getCell" => array("TextBox", 2),
		"getColStyle" => array("TextBox", 1),
		"getRowStyle" => array("TextBox", 1),
		"getStyle" => array("TextBox", 0),
	);
}

class LegendBox extends TextBox 
{
	static $defaultArgs = array(
		"addKey" => array(4, 0, null),
		"addKey2" => array(5, 0, null),
		"setKeySize" => array(3, -1, -1),
		"setKeySpacing" => array(2, -1),
		"setKeyBorder" => array(2, 0),
		"setReverse" => array(1, 1),
		"setLineStyleKey" => array(1, 1),
		"getHTMLImageMap" => array(5, "", "", 0, 0)		
	);
	function getImageCoor2($dataItem, $offsetX = 0, $offsetY = 0) {
		return callmethod("LegendBox.getImageCoor", $this->ptr, $dataItem, $offsetX, $offsetY);
	}
	function addText($text) {
		$this->addKey($text, Transparent, -999);
	}
	function addText2($pos, $text) {
		$this->addKey2($pos, $text, Transparent, -999);
	}
}

class BaseChart extends AutoMethod
{
	static $defaultArgs = array(
		"setBackground" => array(3, -1, 0),
		"setBgImage" => array(2, Center),
		"setDropShadow" => array(4, 0xaaaaaa, 5, 0x7fffffff, 5),
		"setAntiAlias" => array(2, 1, AutoAntiAlias),
		"initDynamicLayer" => array("DrawArea", 0),
		"addTitle2" => array("TextBox", 7, "", 12, TextColor, Transparent, Transparent),
		"addTitle" => array("TextBox", 6, "", 12, TextColor, Transparent, Transparent),
		"addLegend" => array("LegendBox", 5, 1, "", 10),
		"addLegend2" => array("LegendBox", 5, 1, "", 10),
		"getLegend" => array("LegendBox", 0),
		"layoutLegend" => array("LegendBox", 0),
		"getDrawArea" => array("DrawArea", 0),
		"addText" => array("TextBox", 9, "", 8, TextColor, TopLeft, 0, 0),
		"addLine" => array("Line", 6, LineColor, 1),
		"addTable" => array("CDMLTable", 5),
		"dashLineColor" => array(2, DashLine),
		"patternColor2" => array(3, 0, 0),
		"gradientColor2" => array(5, 90, 1, 0, 0),
		"setDefaultFonts" => array(4, "", "", ""),
		"setNumberFormat" => array(3, "~", ".", "-"),
		"makeChart3" => array("DrawArea", 0),
		"getHTMLImageMap" => array(5, "", "", 0, 0),
		"getJsChartModel" => array(1, ""),
		"setRoundedFrame" => array(5, 0xffffff, 10, -1, -1, -1),
		"setThickFrame" => array(4, -1, -1, -1),
		"linearGradientColor" => array(7, 0),
		"linearGradientColor2" => array(6, 0),
		"radialGradientColor" => array(7, 0),
		"radialGradientColor2" => array(6, 0),
	);
	
	function __del__() {
		callmethod("BaseChart.destroy", $this->ptr);
	}
	function patternColor($c, $h = 0, $startX = 0, $startY = 0) {
	    if (!is_array($c))
	        return $this->patternColor2($c, $h, $startX);
		return callmethod("BaseChart.patternColor", $this->ptr, $c, $h, $startX, $startY);
    }
    function gradientColor($startX, $startY = 90, $endX = 1, $endY = 0, $startColor = 0, $endColor = Null) {
		if (is_array($startX))
			return $this->gradientColor2($startX, $startY, $endX, $endY, $startColor);
		return callmethod("BaseChart.gradientColor", $this->ptr, $startX, $startY, $endX, $endY, $startColor, $endColor);
	}
	function makeSession($id, $format = PNG) {
		if (!defined('PHP_VERSION_ID'))
			session_register($id);
		else if (!session_id()) 
			@session_start();
		$_SESSION[$id] = $this->makeChart2($format);
		$ret = "img=".$id."&id=".uniqid(session_id());	
		if (defined('SID'))
			$ret .= "&".SID;
		if (($format == SVG) || ($format == SVGZ))
			$ret .= "&stype=.svg";
		return $ret;
	}
}

class MultiChart extends BaseChart 
{
	function __construct($width, $height, $bgColor = BackgroundColor, $edgeColor = Transparent, $raisedEffect = 0) {
		$this->ptr = callmethod("MultiChart.create", $width, $height, $bgColor, $edgeColor, $raisedEffect);
		$this->charts = array();
		$this->mainChart = null;
		autoDestroy($this);
	}
	function addChart($x, $y, $c) {
		if ($c)	{
			callmethod("MultiChart.addChart", $this->ptr, $x, $y, $c->ptr);
			$this->charts[] = $c;
		}
	}
	function getChart($i = 0) {
		if ($i == -1)
			return $this->mainChart;
		if (($i >= 0) && ($i < count($this->charts)))
			return $this->charts[$i];
		return null;
	}
	function getChartCount() {
		return count($this->charts);
	}
	function setMainChart($c) { 
		$this->mainChart = $c;
		callmethod("MultiChart.setMainChart", $this->ptr, $c->ptr);
	}	
}

class Sector extends AutoMethod
{
	static $defaultArgs = array(
		"setExplode" => array(1, -1),
		"setLabelStyle" => array("TextBox", 3, "", 8, TextColor),
		"setLabelPos" => array(2, -1),
		"setLabelLayout" => array(2, -1),
		"setJoinLine" => array(2, 1),
		"setColor" => array(3, -1, -1),
		"setStyle" => array(3, -1, -1),
		"getImageCoor" => array(2, 0, 0),
		"getLabelCoor" => array(2, 0, 0)
	);
}

class PieChart extends BaseChart 
{
	static $defaultArgs = array(
		"setStartAngle" => array(2, 1),
		"setExplode" => array(2, -1, -1),
		"setExplodeGroup" => array(3, -1),
		"setLabelStyle" => array("TextBox", 3, "", 8, TextColor),
		"setLabelPos" => array(2, -1),
		"setLabelLayout" => array(4, -1, -1, -1),
		"setJoinLine" => array(2, 1),
		"setLineColor" => array(2, -1),
		"setSectorStyle" => array(3, -1, -1),
		"setData" => array(2, null),
		"sector" => array("Sector", 1),
		"set3D2" => array(3, -1, 0)
	);
	
	function __construct($width, $height, $bgColor = BackgroundColor, $edgeColor = Transparent, $raisedEffect = 0) {
		$this->ptr = callmethod("PieChart.create", $width, $height, $bgColor, $edgeColor, $raisedEffect);
		autoDestroy($this);
	}
	function set3D($depth = -1, $angle = -1, $shadowMode = 0) {
		if (is_array($depth))
			$this->set3D2($depth, $angle, $shadowMode);
		else 
			callmethod("PieChart.set3D", $this->ptr, $depth, $angle, $shadowMode);
	}
	function getSector($sectorNo) {
		return $this->sector($sectorNo);
	}
}

class Mark extends TextBox 
{
	function setMarkColor($lineColor, $textColor = -1, $tickColor = -1) {
		callmethod("Mark.setMarkColor", $this->ptr, $lineColor, $textColor, $tickColor);
	}
}

class Axis extends AutoMethod
{
	static $defaultArgs = array(
		"setLabelStyle" => array("TextBox", 4, "", 8, TextColor, 0),
		"setTitle" => array("TextBox", 4, "", 8, TextColor),
		"setTitlePos" => array(2, 3),
		"setColors" => array(4, TextColor, -1, -1),
		"setTickWidth" => array(2, -1),
		"setTickColor" => array(2, -1),
		"setMargin" => array(2, 0),
		"setAutoScale" => array(3, 0.1, 0.1, 0.8),
		"setTickDensity" => array(2, -1),
		"setReverse" => array(1, 1),
		"setLabels2" => array("TextBox", 2, ""),
		"makeLabelTable" => array("CDMLTable", 0),
		"getLabelTable" => array("CDMLTable", 0),
		"setLinearScale3" => array(1, ""),
		"setDateScale3" => array(1, ""),
		"addMark" => array("Mark", 5, "", "", 8),
		"addLabel2" => array(3, 0),
		"getFormattedLabel" => array(2, ""),
		"getAxisImageMap" => array(7, "", "", 0, 0),
		"getHTMLImageMap" => array(5, "", "", 0, 0),
		"setMultiFormat2" => array(4, 1, 1),
		"setLabelStep" => array(4, 0, 0, -0x7fffffff),
		"setFormatCondition" => array(2, 0),
		"setLabelAlignment" => array(2, 3),
		"syncAxis" => array(3, 1, 0),
		"syncScale" => array(3, 1, 0)
	);
	function setMultiFormat($filter1, $format1, $filter2 = 1, $format2 = Null, $labelSpan = 1, $promoteFirst = 1) {
		if (is_null($format2))
			$this->setMultiFormat2($filter1, $format1, $filter2, 1);
		else
			callmethod("Axis.setMultiFormat", $this->ptr, $filter1, $format1, $filter2, $format2, $labelSpan, $promoteFirst);
	}
	function setTickLength($majorTickLen, $minorTickLen = Null) {
		if (is_null($minorTickLen))
			callmethod("Axis.setTickLength", $this->ptr, $majorTickLen);
		else
			$this->setTickLength2($majorTickLen, $minorTickLen);
	}
	function setTopMargin($topMargin) {
		$this->setMargin($topMargin);
	}	
	function setLabels($labels, $formatString = Null) {
		if (is_null($formatString))
			return new TextBox(callmethod("Axis.setLabels", $this->ptr, $labels));
		else
			return $this->setLabels2($labels, $formatString);
	}
	function setLinearScale($lowerLimit = Null, $upperLimit = Null, $majorTickInc = 0, $minorTickInc = 0) {
		if (is_null($lowerLimit))
			$this->setLinearScale3();
		else if (is_null($upperLimit))
			$this->setLinearScale3($lowerLimit);
		else if (is_array($majorTickInc))
			$this->setLinearScale2($lowerLimit, $upperLimit, $majorTickInc);
		else	
			callmethod("Axis.setLinearScale", $this->ptr, $lowerLimit, $upperLimit, $majorTickInc, $minorTickInc);
	}	
	function setLogScale($lowerLimit = Null, $upperLimit = Null, $majorTickInc = 0, $minorTickInc = 0) {
		if (is_null($lowerLimit))
			$this->setLogScale3();
		else if (is_null($upperLimit))
			$this->setLogScale3($lowerLimit);
		else if (is_array($majorTickInc))
			$this->setLogScale2($lowerLimit, $upperLimit, $majorTickInc);
		else	
			callmethod("Axis.setLogScale", $this->ptr, $lowerLimit, $upperLimit, $majorTickInc, $minorTickInc);
	}	
	function setLogScale2($lowerLimit, $upperLimit, $labels = 0) {
		if (is_array($labels))
			callmethod("Axis.setLogScale2", $this->ptr, $lowerLimit, $upperLimit, $labels);
		else
			#compatibility with ChartDirector Ver 2.5
			$this->setLogScale($lowerLimit, $upperLimit, $labels);
	}
	function setLogScale3($formatString = "") {
		if (!is_string($formatString)) {
			#compatibility with ChartDirector Ver 2.5
			if ($formatString)
				$this->setLogScale3();
			else
				$this->setLinearScale3();
		}
		else
			callmethod("Axis.setLogScale3", $this->ptr, $formatString);
	}	
	function setDateScale($lowerLimit = Null, $upperLimit = Null, $majorTickInc = 0, $minorTickInc = 0) {
		if (is_null($lowerLimit))
			$this->setDateScale3();
		else if (is_null($upperLimit))
			$this->setDateScale3($lowerLimit);
		else if (is_array($majorTickInc))
			$this->setDateScale2($lowerLimit, $upperLimit, $majorTickInc);
		else	
			callmethod("Axis.setDateScale", $this->ptr, $lowerLimit, $upperLimit, $majorTickInc, $minorTickInc);
	}	
}

class AngularAxis extends AutoMethod
{
	static $defaultArgs = array(
		"setLabelStyle" => array("TextBox", 4, "", 8, TextColor, 0),
		"setReverse" => array(1, 1),
		"setLabels2" => array("TextBox", 2, ""),
		"addZone2" => array(4, -1),
		"getAxisImageMap" => array(7, "", "", 0, 0),
		"getHTMLImageMap" => array(5, "", "", 0, 0)
	);
		
	function setLabels($labels, $formatString = Null) {
		if (is_null($formatString))
			return new TextBox(callmethod("AngularAxis.setLabels", $this->ptr, $labels));
		else
			return $this->setLabels2($labels, $formatString);
	}
	function setLinearScale($lowerLimit, $upperLimit, $majorTickInc = 0, $minorTickInc = 0) {
		if (is_array($majorTickInc))
			$this->setLinearScale2($lowerLimit, $upperLimit, $majorTickInc);
		else	
			callmethod("AngularAxis.setLinearScale", $this->ptr, $lowerLimit, $upperLimit, $majorTickInc, $minorTickInc);
	}	
	function addZone($startValue, $endValue, $startRadius, $endRadius = -1, $fillColor = Null, $edgeColor = -1) {
		if (is_null($fillColor))
			$this->addZone2($startValue, $endValue, $startRadius, $endRadius);
		else
			callmethod("AngularAxis.addZone", $this->ptr, $startValue, $endValue, $startRadius, $endRadius, $fillColor, $edgeColor);
	}
}

class ColorAxis extends Axis 
{
	static $defaultArgs = array(
		"setColorGradient" => array(4, 1, null, -1, -1),
		"setColorScale" => array(3, -1, -1),
		"setCompactAxis" => array(1, 1),
		"setAxisBorder" => array(2, 0),
		"setBoundingBox" => array(3, Transparent, 0),
		"setRoundedCorners" => array(4, 10, -1, -1, -1)
	);
}

class DataSet extends AutoMethod
{
	static $defaultArgs = array(
		"setDataColor" => array(4, -1, -1, -1, -1),
		"setUseYAxis2" => array(1, 1),
		"setDataLabelStyle" => array("TextBox", 4, "", 8, TextColor, 0),
		"setDataSymbol4" => array(4, 11, -1, -1)
	);	

	function setDataSymbol($symbol, $size = Null, $fillColor = -1, $edgeColor = -1, $lineWidth = 1) {
		if (is_array($symbol)) {
			if (is_null($size))
				$size = 11;
			$this->setDataSymbol4($symbol, $size, $fillColor, $edgeColor);
			return;
		}
	    if (!is_numeric($symbol))
        	return $this->setDataSymbol2($symbol);
        if (is_null($size))
        	$size = 5;
		callmethod("DataSet.setDataSymbol", $this->ptr, $symbol, $size, $fillColor, $edgeColor, $lineWidth);
	}
	function setDataSymbol2($image) {
	    if (!is_string($image))
        	return $this->setDataSymbol3($image);
		callmethod("DataSet.setDataSymbol2", $this->ptr, $image);
	}
}

class Layer extends AutoMethod
{
	static $defaultArgs = array(
		"setBorderColor" => array(2, 0),
		"set3D" => array(2, -1, 0),
		"addDataSet" => array("DataSet", 3, -1, ""),
		"addDataGroup" => array(1, ""),
		"getDataSet" => array("DataSet", 1),
		"getDataSetByZ" => array("DataSet", 1),
		"setUseYAxis2" => array(1, 1),
		"setLegendOrder" => array(2, -1),
		"getXIndexOf" => array(2, 0),
		"setDataLabelStyle" => array("TextBox", 4, "", 8, TextColor, 0),
		"setAggregateLabelStyle" => array("TextBox", 4, "", 8, TextColor, 0),
		"addCustomDataLabel" => array("TextBox", 7, "", 8, TextColor, 0),
		"addCustomAggregateLabel" => array("TextBox", 6, "", 8, TextColor, 0),
		"addCustomGroupLabel" => array("TextBox", 7, "", 8, TextColor, 0),
		"getImageCoor2" => array(3, 0, 0),
		"getHTMLImageMap" => array(5, "", "", 0, 0),
		"setHTMLImageMap" => array(3, "", ""),
		"moveFront" => array(1, null),
		"moveBack" => array(1, null)
	);

	function setXData($xData, $maxValue = Null) {
		if (is_null($maxValue))
			callmethod("Layer.setXData", $this->ptr, $xData);
		else
			$this->setXData2($xData, $maxValue);
	}
	function getYCoor($v, $yAxis = 1) {
		if (is_object($yAxis))
			return callmethod("Layer.getYCoor2", $this->ptr, $v, $yAxis->ptr);
		else
			return callmethod("Layer.getYCoor", $this->ptr, $v, $yAxis);
	}
	function yZoneColor($threshold, $belowColor, $aboveColor, $yAxis = 1) {
		if (is_object($yAxis))
			return callmethod("Layer.yZoneColor2", $this->ptr, $threshold, $belowColor, $aboveColor, $yAxis->ptr);
		else
			return callmethod("Layer.yZoneColor", $this->ptr, $threshold, $belowColor, $aboveColor, $yAxis);
	}
	function getImageCoor($dataSet, $dataItem = Null, $offsetX = 0, $offsetY = 0) {
		if (is_null($dataItem))
			return $this->getImageCoor2($dataSet, $offsetX, $offsetY);
		return callmethod("Layer.getImageCoor", $this->ptr, $dataSet, $dataItem, $offsetX, $offsetY);
	}
}

class BarLayer extends Layer 
{
	static $defaultArgs = array(
		"setBarGap" => array(2, 0.2),
		"setBarWidth" => array(2, -1),
		"setRoundedCorners" => array(4, -0x7fffffff, -0x7fffffff, -0x7fffffff, -0x7fffffff),
		"setIconSize" => array(2, -1),
		"setOverlapRatio" => array(2, 1),
		"setBarShape2" => array(3, -1, -1)
	);
	
	function setBarShape($shape, $dataGroup = -1, $dataItem = -1) {
		if (is_array($shape))
			$this->setBarShape2($shape, $dataGroup, $dataItem);
		else
			callmethod("BarLayer.setBarShape", $this->ptr, $shape, $dataGroup, $dataItem);
	}
}

class LineLayer extends Layer 
{
	static $defaultArgs = array(
		"setGapColor" => array(2, -1),
		"setSymbolScale" => array(4, PixelScale, null, PixelScale),
		"setFastLineMode" => array(1, 1),
		"getLine" => array(1, 0)
	);
}

class ScatterLayer extends LineLayer 
{
}

class InterLineLayer extends LineLayer 
{
	function setGapColor($gapColor12, $gapColor21 = -1) {
		return callmethod("InterLineLayer.setGapColor", $this->ptr, $gapColor12, $gapColor21);
	}
}

class SplineLayer extends LineLayer 
{
}

class StepLineLayer extends LineLayer 
{
}

class AreaLayer extends Layer 
{
}

class TrendLayer extends Layer 
{
	static $defaultArgs = array(
		"addConfidenceBand" => array(7, Transparent, 1, -1, -1, -1),
		"addPredictionBand" => array(7, Transparent, 1, -1, -1, -1),
	);
}

class BaseBoxLayer extends Layer
{
	static $defaultArgs = array(
		"setRoundedCorners" => array(4, -0x7fffffff, -0x7fffffff, -0x7fffffff, -0x7fffffff)
	);
}

class HLOCLayer extends BaseBoxLayer 
{
	function setColorMethod($colorMethod, $riseColor, $fallColor = -1, $leadValue = -1.7E308) {
		callmethod("HLOCLayer.setColorMethod", $this->ptr, $colorMethod, $riseColor, $fallColor, $leadValue);
	}
}

class CandleStickLayer extends BaseBoxLayer 
{
	static $defaultArgs = array(
		"setExtraColors" => array(5, -1.7E308)
	);
}

class BoxWhiskerLayer extends BaseBoxLayer 
{
	static $defaultArgs = array(
		"setBoxColors" => array(2, null),
		"addPredictionBand" => array(7, Transparent, 1, -1, -1, -1),
	);
}

class VectorLayer extends Layer 
{
	static $defaultArgs = array(
		"setVector" => array(3, PixelScale),
		"setIconSize" => array(2, 0),
		"setVectorMargin" => array(2, NoValue)
	);
	
	function setArrowHead($width, $height = 0) {
		if (is_array($width))
			$this->setArrowHead2($width);
		else
			callmethod("VectorLayer.setArrowHead", $this->ptr, $width, $height);
	}
}

class DiscreteHeatMapLayer extends Layer
{
	function __construct($ptr) {
		parent::__construct($ptr);
		$this->colorAxis = $this->colorAxis();
	}
	static $defaultArgs = array(
		"setColorAxis" => array("ColorAxis", 5),
		"colorAxis" => array("ColorAxis", 0)
	);
}
	
class ContourLayer extends Layer 
{
	static $defaultArgs = array(
		"setContourColor" => array(2, -1),
		"setContourWidth" => array(2, -1),
		"setColorAxis" => array("ColorAxis", 5),
		"colorAxis" => array("ColorAxis", 0),
		"setContourLabelStyle" => array(3, "", 8, TextColor),
		"addCustomContour" => array(7, "", 8, TextColor)
	);
	
	function __construct($ptr) {
		parent::__construct($ptr);
		$this->colorAxis = $this->colorAxis();
	}
	function setExactContour($contour = true, $markContour = Null) {
		if (is_null($markContour)) 
			$markContour = $contour;
		callmethod("ContourLayer.setExactContour", $this->ptr, $contour, $markContour); 
	}
}

class PlotArea extends AutoMethod
{
	static $defaultArgs = array(
		"setBackground" => array(3, -1, -1),
		"setBackground2" => array(2, Center),
		"set4QBgColor" => array(5, -1),
		"setAltBgColor" => array(4, -1),
		"setGridColor" => array(4, Transparent, -1, -1),
		"setGridWidth" => array(4, -1, -1, -1),
		"moveGridBefore" => array(1, null)
	);
}

class XYChart extends BaseChart 
{	
	static $defaultArgs = array(
		"yAxis" => array("Axis", 0),	
		"yAxis2" => array("Axis", 0),	
		"syncYAxis" => array(2, 1, 0),	
		"setYAxisOnRight" => array(1, 1),
		"setXAxisOnTop" => array(1, 1),
		"xAxis" => array("Axis", 0),	
		"xAxis2" => array("Axis", 0),	
		"addAxis" => array("Axis", 2),
		"swapXY" => array(1, 1),
		"setPlotArea" => array("PlotArea", 9, Transparent, -1, -1, 0xc0c0c0, Transparent),
		"getPlotArea" => array("PlotArea", 0),
		"setClipping" => array(1, 0),
		"addBarLayer2" => array("BarLayer", 2, Side, 0),
		"addBarLayer3" => array("BarLayer", 4, null, null, 0),
		"addLineLayer2" => array("LineLayer", 2, Overlay, 0),
		"addAreaLayer2" => array("AreaLayer", 2, Stack, 0),
		"addHLOCLayer2" => array("HLOCLayer", 0),
		"addHLOCLayer3" => array("HLOCLayer", 8, -1, NoValue),
		"addScatterLayer" => array("ScatterLayer", 7, "", SquareSymbol, 5, -1, -1),
		"addCandleStickLayer" => array("CandleStickLayer", 7, 0xffffff, 0x0, LineColor),
		"addBoxWhiskerLayer" => array("BoxWhiskerLayer", 8, null, null, null, -1, LineColor, -1),
		"addBoxWhiskerLayer2" => array("BoxWhiskerLayer", 8, null, null, null, null, 0.5, null),
		"addBoxLayer" => array("BoxWhiskerLayer", 4, -1, ""),
		"addTrendLayer" => array("TrendLayer", 4, -1, "", 0),
		"addTrendLayer2" => array("TrendLayer", 5, -1, "", 0),
		"addSplineLayer" => array("SplineLayer", 3, null, -1, ""),
		"addStepLineLayer" => array("StepLineLayer", 3, null, -1, ""),
		"addInterLineLayer" => array("InterLineLayer", 4, -1),
		"addVectorLayer" => array("VectorLayer", 7, PixelScale, -1, ""),
		"addContourLayer" => array("ContourLayer", 3),
		"addDiscreteHeatMapLayer" => array("DiscreteHeatMapLayer", 2),
		"addDiscreteHeatMapLayer2" => array("DiscreteHeatMapLayer", 3),
		"getLayer" => array("Layer", 1),
		"getLayerByZ" => array("Layer", 1),
		"setAxisAtOrigin" => array(2, XYAxisAtOrigin, 0),
		"setTrimData" => array(2, 0x7fffffff),
		"packPlotArea" => array(6, 0, 0),
		"getYCoor" => array(2, null),
		"getYValue" => array(2, null),
		"yZoneColor" => array(4, null),
		"yScaleColor" => array(2, null)		
	);
	
	function __construct($width, $height, $bgColor = BackgroundColor, $edgeColor = Transparent, $raisedEffect = 0) {
		$this->ptr = callmethod("XYChart.create", $width, $height, $bgColor, $edgeColor, $raisedEffect);
		$this->xAxis = $this->xAxis();
		$this->xAxis2 = $this->xAxis2();
		$this->yAxis = $this->yAxis();
		$this->yAxis2 = $this->yAxis2();
		autoDestroy($this);
	}
	function addBarLayer($data = Null, $color = -1, $name = "", $depth = 0) {
		if (!is_null($data))
			return new BarLayer(callmethod("XYChart.addBarLayer", $this->ptr, $data, $color, $name, $depth));
		else
			return $this->addBarLayer2();
	}
	function addLineLayer($data = Null, $color = -1, $name = "", $depth = 0) {
		if (!is_null($data))
			return new LineLayer(callmethod("XYChart.addLineLayer", $this->ptr, $data, $color, $name, $depth));
		else
			return $this->addLineLayer2();
	}
	function addAreaLayer($data = Null, $color = -1, $name = "", $depth = 0) {
		if (!is_null($data))
			return new AreaLayer(callmethod("XYChart.addAreaLayer", $this->ptr, $data, $color, $name, $depth));
		else
			return $this->addAreaLayer2();
	}
	function addHLOCLayer($highData = Null, $lowData = Null, $openData = Null, $closeData = Null, $color = -1) {
		if (!is_null($highData))
			return $this->addHLOCLayer3($highData, $lowData, $openData, $closeData, $color, $color);
		else
			return $this->addHLOCLayer2();
	}
}

class ThreeDChart extends BaseChart 
{
	static $defaultArgs = array(
		"setViewAngle" => array(3, 0, 0),	
		"xAxis" => array("Axis", 0),	
		"yAxis" => array("Axis", 0),
		"zAxis" => array("Axis", 0),	
		"setColorAxis" => array("ColorAxis", 5),	
		"colorAxis" => array("ColorAxis", 0),
		"setWallColor" => array(4, -1, -1, -1),
		"setWallThickness" => array(3, -1, -1),
		"setWallGrid" => array(6, -1, -1, -1, -1, -1)
	);
}

class SurfaceChart extends ThreeDChart 
{
	static $defaultArgs = array(
		"setInterpolation" => array(3, -1, 1),	
		"setShadingMode" => array(2, 1),	
		"setSurfaceAxisGrid" => array(4, -1, -1, -1),
		"setSurfaceDataGrid" => array(2, -1),
		"setContourColor" => array(2, -1),
		"setWContourColor" => array(2, -1),
		"addXYProjection" => array(1, 0),
		"addSurfaceZone" => array(7, Transparent, 1)
	);
	
	function __construct($width, $height, $bgColor = BackgroundColor, $edgeColor = Transparent, $raisedEffect = 0) {
		$this->ptr = callmethod("SurfaceChart.create", $width, $height, $bgColor, $edgeColor, $raisedEffect);
		$this->xAxis = $this->xAxis();
		$this->yAxis = $this->yAxis();
		$this->zAxis = $this->zAxis();
		$this->colorAxis = $this->colorAxis();
		autoDestroy($this);
	}

	function setData($xData, $yData, $zData, $wData = null) {
		callmethod("SurfaceChart.setData2", $this->ptr, $xData, $yData, $zData, $wData); 
	}
	function setInterpolation($xSamples, $ySamples = -1, $isSmooth = 1, $isColorSmooth = null) {
		if (is_null($isColorSmooth))
			$isColorSmooth = $isSmooth;
		callmethod("SurfaceChart.setInterpolation2", $this->ptr, $xSamples, $ySamples, $isSmooth, $isColorSmooth); 
	}
	function addSurfaceLine($x1, $y1, $x2, $y2, $color, $lineWidth = 1, $side = 0) {
		$this->addSurfaceLine2(array($x1, $x2), array($y1, $y2), $color, $lineWidth, $side);
	}
	function addSurfaceLine2($x, $y, $color, $lineWidth = 1, $side = 0) {
		callmethod("SurfaceChart.addSurfaceLine", $this->ptr, $x, $y, $color, $lineWidth, $side);
	}
}

class ThreeDScatterGroup extends AutoMethod
{
	static $defaultArgs = array(
		"setDataSymbol4" => array(4, 11, -1, -1),
		"setDropLine" => array(2, LineColor, 1),
		"setLegendIcon" => array(3, -1, -1)
	);

	function setDataSymbol($symbol, $size = Null, $fillColor = -1, $edgeColor = -1, $lineWidth = 1) {
		if (is_array($symbol)) {
			if (is_null($size))
				$size = 11;
			$this->setDataSymbol4($symbol, $size, $fillColor, $edgeColor);
			return;
		}
	    if (!is_numeric($symbol))
        	return $this->setDataSymbol2($symbol);
        if (is_null($size))
        	$size = 5;
		callmethod("ThreeDScatterGroup.setDataSymbol", $this->ptr, $symbol, $size, $fillColor, $edgeColor, $lineWidth);
	}
	function setDataSymbol2($image) {
	    if (!is_string($image))
        	return $this->setDataSymbol3($image);
		callmethod("ThreeDScatterGroup.setDataSymbol2", $this->ptr, $image);
	}
}

class ThreeDScatterChart extends ThreeDChart 
{	
	static $defaultArgs = array(
		"addScatterGroup" => array("ThreeDScatterGroup", 8, "", CircleSymbol, 5, -1, -1)
	);
	
	function __construct($width, $height, $bgColor = BackgroundColor, $edgeColor = Transparent, $raisedEffect = 0) {
		$this->ptr = callmethod("ThreeDScatterChart.create", $width, $height, $bgColor, $edgeColor, $raisedEffect);
		$this->xAxis = $this->xAxis();
		$this->yAxis = $this->yAxis();
		$this->zAxis = $this->zAxis();
		$this->colorAxis = $this->colorAxis();
		autoDestroy($this);
	}
}

class PolarLayer extends AutoMethod
{
	static $defaultArgs = array(
		"setData" => array(3, -1, ""),
		"setSymbolScale" => array(2, PixelScale),
		"getImageCoor" => array(3, 0, 0),
		"getHTMLImageMap" => array(5, "", "", 0, 0),
		"setDataLabelStyle" => array("TextBox", 4, "", 8, TextColor, 0),
		"addCustomDataLabel" => array("TextBox", 6, "", 8, TextColor, 0),
		"setDataSymbol4" => array(4, 11, -1, -1),
		"setHTMLImageMap" => array(3, "", "")
	);

	function setDataSymbol($symbol, $size = Null, $fillColor = -1, $edgeColor = -1, $lineWidth = 1) {
	    if (is_array($symbol)) {
	    	if (is_null($size))
	    		$size = 11;
	    	$this->setDataSymbol4($symbol, $size, $fillColor, $edgeColor);
	    	return;
	    }
	    if (!is_numeric($symbol))
        	return $this->setDataSymbol2($symbol);
		if (is_null($size))
			$size = 7;
		callmethod("PolarLayer.setDataSymbol", $this->ptr, $symbol, $size, $fillColor, $edgeColor, $lineWidth);
	}
	function setDataSymbol2($image) {
	    if (!is_string($image))
        	return $this->setDataSymbol3($image);
		callmethod("PolarLayer.setDataSymbol2", $this->ptr, $image);
	}
}

class PolarAreaLayer extends PolarLayer 
{
}

class PolarLineLayer extends PolarLayer 
{
	static $defaultArgs = array(
		"setGapColor" => array(2, -1)
	);
}

class PolarSplineLineLayer extends PolarLineLayer 
{
}

class PolarSplineAreaLayer extends PolarAreaLayer 
{
}

class PolarVectorLayer extends PolarLayer 
{	
	static $defaultArgs = array(
		"setVector" => array(3, PixelScale),
		"setIconSize" => array(2, 0),
		"setVectorMargin" => array(2, NoValue)
	);

	function setArrowHead($width, $height = 0) {
		if (is_array($width))
			$this->setArrowHead2($width);
		else
			callmethod("PolarVectorLayer.setArrowHead", $this->ptr, $width, $height);
	}
}

class PolarChart extends BaseChart 
{
	static $defaultArgs = array(
		"setPlotArea" => array(6, Transparent, Transparent, 1),	
		"setPlotAreaBg" => array(3, -1, 1),	
		"setGridColor" => array(4, LineColor, 1, LineColor, 1),
		"setGridStyle" => array(2, 1),
		"setStartAngle" => array(2, 1),
		"angularAxis" => array("AngularAxis", 0),
		"radialAxis" => array("Axis", 0),	
		"addAreaLayer" => array("PolarAreaLayer", 3, -1, ""),
		"addLineLayer" => array("PolarLineLayer", 3, -1, ""),
		"addSplineLineLayer" => array("PolarSplineLineLayer", 3, -1, ""),
		"addSplineAreaLayer" => array("PolarSplineAreaLayer", 3, -1, ""),
		"addVectorLayer" => array("PolarVectorLayer", 7, PixelScale, -1, "")
	);
	
	function __construct($width, $height, $bgColor = BackgroundColor, $edgeColor = Transparent, $raisedEffect = 0) {
		$this->ptr = callmethod("PolarChart.create", $width, $height, $bgColor, $edgeColor, $raisedEffect);
		$this->angularAxis = $this->angularAxis();
		$this->radialAxis = $this->radialAxis();
		autoDestroy($this);
	}
}

class TreeMapNode extends AutoMethod
{
	static $defaultArgs = array(
		"setData" => array(3, null, null),
		"setColors" => array(3, -1, -0x7fffffff),
		"setLabelFormat" => array("TextBox", 5, "{label}", "", 8, TextColor, TopLeft),
		"getNode" => array("TreeMapNode", 1)
	);	
	
	function setLayoutMethod($layoutMethod, $layoutDirection = -1, $swapXY = null) {
		if (is_null($swapXY))
			$swapXY = 0;
		elseif ($swapXY)
			$swapXY = 1;
		else
			$swapXY = -1;
		callmethod("TreeMapNode.setLayoutMethod", $this->ptr, $layoutMethod, $layoutDirection, $swapXY);
	}
}
	
class TreeMapChart extends BaseChart
{
	static $defaultArgs = array(
		"getRootNode" => array("TreeMapNode", 0),
		"getLevelPrototype" => array("TreeMapNode", 1)
	);
	
	function __construct($width, $height, $bgColor = BackgroundColor, $edgeColor = Transparent, $raisedEffect = 0) {
		$this->ptr = callmethod("TreeMapChart.create", $width, $height, $bgColor, $edgeColor, $raisedEffect);
		autoDestroy($this);
	}
}
	
class PyramidLayer extends AutoMethod
{
	static $defaultArgs = array(
		"setCenterLabel" => array("TextBox", 4, "{skip}", "{skip}", -1, -1),
		"setRightLabel" => array("TextBox", 4, "{skip}", "{skip}", -1, -1),
		"setLeftLabel" => array("TextBox", 4, "{skip}", "{skip}", -1, -1),
		"setJoinLine" => array(2, -1),
		"setJoinLineGap" => array(3, -0x7fffffff, -0x7fffffff),
		"setLayerBorder" => array(2, -1)
	);	
}

class PyramidChart extends BaseChart 
{
	static $defaultArgs = array(
		"setFunnelSize" => array(6, 0.2, 0.3),
		"setData" => array(2, null),
		"setCenterLabel" => array("TextBox", 4, "{skip}", "{skip}", -1, -1),
		"setRightLabel" => array("TextBox", 4, "{skip}", "{skip}", -1, -1),
		"setLeftLabel" => array("TextBox", 4, "{skip}", "{skip}", -1, -1),
		"setViewAngle" => array(3, 0, 0),
		"setLighting" => array(4, 0.5, 0.5, 1, 8),
		"setJoinLine" => array(2, -1),
		"setJoinLineGap" => array(3, -0x7fffffff, -0x7fffffff),
		"setLayerBorder" => array(2, -1),
		"getLayer" => array("PyramidLayer", 1)
	);
	
	function __construct($width, $height, $bgColor = BackgroundColor, $edgeColor = Transparent, $raisedEffect = 0) {
		$this->ptr = callmethod("PyramidChart.create", $width, $height, $bgColor, $edgeColor, $raisedEffect);
		autoDestroy($this);
	}
}

class MeterPointer extends AutoMethod
{
	static $defaultArgs = array(
		"setColor" => array(2, -1),
		"setShape2" => array(3, NoValue, NoValue),
		"setShapeAndOffset2" => array(4, NoValue, NoValue, NoValue)
	);

	function setShape($pointerType, $lengthRatio = NoValue, $widthRatio = NoValue) {
		if (is_array($pointerType))
			$this->setShape2($pointerType, $lengthRatio, $widthRatio);
		else
			callmethod("MeterPointer.setShape", $this->ptr, $pointerType, $lengthRatio, $widthRatio);
	}
	function setShapeAndOffset($pointerType, $startOffset = NoValue, $endOffset = NoValue, $widthRatio = NoValue) {
		if (is_array($pointerType))
			$this->setShapeAndOffset2($pointerType, $startOffset, $endOffset, $widthRatio);
		else
			callmethod("MeterPointer.setShapeAndOffset", $this->ptr, $pointerType, $startOffset, $endOffset, $widthRatio); 
	}
}

class BaseMeter extends BaseChart
{
	static $defaultArgs = array(
		"addPointer" => array("MeterPointer", 3, LineColor, -1),
		"setScale3" => array(4, ""),
		"setLabelStyle" => array("TextBox", 4, "bold", -1, TextColor, 0),
		"setLabelPos" => array(2, 0),
		"setTickLength" => array(3, -0x7fffffff, -0x7fffffff),
		"setLineWidth" => array(4, 1, 1, 1),
		"setMeterColors" => array(3, -1, -1),
		"addColorScale" => array(6, -0x7fffffff, -0x7fffffff, -0x7fffffff, -0x7fffffff, -1)
	);

	function setScale($lowerLimit, $upperLimit, $majorTickInc = 0, $minorTickInc = 0, $microTickInc = 0) {
		if (is_array($majorTickInc)) {
			if ($minorTickInc != 0)
				$this->setScale3($lowerLimit, $upperLimit, $majorTickInc, $minorTickInc);
			else
				$this->setScale2($lowerLimit, $upperLimit, $majorTickInc);
		} else
			callmethod("BaseMeter.setScale", $this->ptr, $lowerLimit, $upperLimit, $majorTickInc, $minorTickInc, $microTickInc);
	}
}

class AngularMeter extends BaseMeter
{
	static $defaultArgs = array(
		"addRing" => array(4, -1),
		"addRingSector" => array(6, -1),
		"setCap" => array(3, LineColor),
		"addZone2" => array(4, -1),
		"addScaleBackground" => array(7, 0, -1, -0x7fffffff, NoValue, NoValue),
		"addGlare" => array(5, NoValue, 135, 0, NoValue, 0.13),
		"setCap2" => array(7, 0x888888, 0x000000, 0x888888, NoValue, NoValue, NoValue, NoValue),
		"addPointer2" => array("MeterPointer", 7, -1, TriangularPointer2, NoValue, NoValue, NoValue),
		"relativeRadialGradient" => array(2, -1),
		"relativeLinearGradient" => array(3, 0, -1)
	);
	
	function __construct($width, $height, $bgColor = BackgroundColor, $edgeColor = Transparent, $raisedEffect = 0) {
		$this->ptr = callmethod("AngularMeter.create", $width, $height, $bgColor, $edgeColor, $raisedEffect);
		autoDestroy($this);
	}
	function addZone($startValue, $endValue, $startRadius, $endRadius = -1, $fillColor = Null, $edgeColor = -1) {
		if (is_null($fillColor))
			$this->addZone2($startValue, $endValue, $startRadius, $endRadius);
		else
			callmethod("AngularMeter.addZone", $this->ptr, $startValue, $endValue, $startRadius, $endRadius, $fillColor, $edgeColor);
	}
}

class LinearMeter extends BaseMeter
{
	static $defaultArgs = array(
		"setMeter" => array(6, Left, 0),
		"setRail" => array(3, 2, 6),
		"addZone" => array("TextBox", 4, ""),
		"addBar" => array("TextBox", 5, 0, 0)
	);
		
	function __construct($width, $height, $bgColor = BackgroundColor, $edgeColor = Transparent, $raisedEffect = 0) {
		$this->ptr = callmethod("LinearMeter.create", $width, $height, $bgColor, $edgeColor, $raisedEffect);
		autoDestroy($this);
	}
}

function getCopyright() {
	return callmethod("getCopyright");
}

function getVersion() {
	return callmethod("getVersion");
}

function getDescription() {
	return cdFilterMsg(callmethod("getDescription"));
}

function getBootLog() {
	return cdFilterMsg(callmethod("getBootLog"));
}

function libgTTFTest($font = "", $fontIndex = 0, $fontHeight = 8, $fontWidth = 8, $angle = 0) {
    return cdFilterMsg(callmethod("testFont", $font, $fontIndex, $fontHeight, $fontWidth, $angle));
}

function testFont($font = "", $fontIndex = 0, $fontHeight = 8, $fontWidth = 8, $angle = 0) {
    return cdFilterMsg(callmethod("testFont", $font, $fontIndex, $fontHeight, $fontWidth, $angle));
}

function setLicenseCode($licCode) {
    return callmethod("setLicenseCode", $licCode);
}

function chartTime($y, $m = Null, $d = 1, $h = 0, $n = 0, $s = 0) {
	if (is_null($m))
		return chartTime2($y);
	else
	    return callmethod("chartTime", $y, $m, $d, $h, $n, $s);
}

function chartTime2($t) {
    return callmethod("chartTime2", $t);
}

function getChartYMD($t) {
	return callmethod("getChartYMD", $t);
}
	
function getChartWeekDay($t) {
	return ((int)($t / 86400 + 1)) % 7;
}

class RanTable extends AutoMethod
{
	static $defaultArgs = array(
		"setCol2" => array(6, -1E+308, 1E+308),
		"setDateCol" => array(4, 0),
		"setHLOCCols" => array(6, 0, 1E+308)
	);
		
	function __construct($seed, $noOfCols, $noOfRows) {
		$this->ptr = callmethod("RanTable.create", $seed, $noOfCols, $noOfRows);
		autoDestroy($this);
	}
	function __del__() {
		callmethod("RanTable.destroy", $this->ptr);
	}
	function setCol($colNo, $minValue, $maxValue, $p4 = Null, $p5 = -1E+308, $p6 = 1E+308) {
		if (is_null($p4))
			callmethod("RanTable.setCol", $this->ptr, $colNo, $minValue, $maxValue);
		else
			$this->setCol2($colNo, $minValue, $maxValue, $p4, $p5, $p6);
	}
}

class RanSeries extends AutoMethod
{
	static $defaultArgs = array(
		"getSeries2" => array(6, -1E+308, 1E+308),
		"getDateSeries" => array(4, 0),
	);

	function __construct($seed) {
		$this->ptr = callmethod("RanSeries.create", $seed);
		autoDestroy($this);
	}
	function __del__() {
		callmethod("RanSeries.destroy", $this->ptr);
	}
	function getSeries($len, $minValue, $maxValue, $p4 = Null, $p5 = -1E+308, $p6 = 1E+308) {
		if (is_null($p4))
			return callmethod("RanSeries.getSeries", $this->ptr, $len, $minValue, $maxValue);
		else
			return $this->getSeries2($len, $minValue, $maxValue, $p4, $p5, $p6);
	}
}

class FinanceSimulator extends AutoMethod
{
	function __construct($seed, $startTime, $endTime, $resolution) {
		if (is_int($seed))
			$this->ptr = callmethod("FinanceSimulator.create", $seed, $startTime, $endTime, $resolution);
		else
			$this->ptr = callmethod("FinanceSimulator.create2", $seed, $startTime, $endTime, $resolution);
		autoDestroy($this);
	}
	function __del__() {
		callmethod("FinanceSimulator.destroy", $this->ptr);
	}
}

#[AllowDynamicProperties]
class ArrayMath
{
	function __construct($a) {
		$this->ptr = callmethod("ArrayMath.create", $a);
		autoDestroy($this);
	}
	function __del__() {
		callmethod("ArrayMath.destroy", $this->ptr);
	}
	
	function add($b) { 
		if (!is_array($b)) 
			$this->add2($b);
		else 
			callmethod("ArrayMath.add", $this->ptr, $b);
		return $this;
	}
	function add2($b) {
		callmethod("ArrayMath.add2", $this->ptr, $b);
		return $this;
	}
	function sub($b) {
		if (!is_array($b)) 
			$this->sub2($b);
		else
			callmethod("ArrayMath.sub", $this->ptr, $b);
		return $this;
	}
	function sub2($b) {
		callmethod("ArrayMath.sub2", $this->ptr, $b);
		return $this;
	}
	function mul($b) {
		if (!is_array($b)) 
			$this->mul2($b);
		else
			callmethod("ArrayMath.mul", $this->ptr, $b);
		return $this;
	}
	function mul2($b) {
		callmethod("ArrayMath.mul2", $this->ptr, $b);
		return $this;
	}
	function div($b) {
		if (!is_array($b)) 
			$this->div2($b);
		else
			callmethod("ArrayMath.div", $this->ptr, $b);
		return $this;
	}
	function div2($b) {
		callmethod("ArrayMath.div2", $this->ptr, $b);
		return $this;
	}
	function financeDiv($b, $zeroByZeroValue) {
		callmethod("ArrayMath.financeDiv", $this->ptr, $b, $zeroByZeroValue);
		return $this;
	}
	function shift($offset = 1, $fillValue = NoValue) {
		callmethod("ArrayMath.shift", $this->ptr, $offset, $fillValue);
		return $this;
	}
	function delta($offset = 1) {
		callmethod("ArrayMath.delta", $this->ptr, $offset);
		return $this;
	}
	function rate($offset = 1) {
		callmethod("ArrayMath.rate", $this->ptr, $offset);
		return $this;
	}
	function abs() {
		callmethod("ArrayMath.abs", $this->ptr);
		return $this;
	}
	function acc() {
		callmethod("ArrayMath.acc", $this->ptr);
		return $this;
	}
	
	function selectGTZ($b = Null, $fillValue = 0) { callmethod("ArrayMath.selectGTZ", $this->ptr, $b, $fillValue); return $this; }
	function selectGEZ($b = Null, $fillValue = 0) { callmethod("ArrayMath.selectGEZ", $this->ptr, $b, $fillValue); return $this; }
	function selectLTZ($b = Null, $fillValue = 0) { callmethod("ArrayMath.selectLTZ", $this->ptr, $b, $fillValue); return $this; }
	function selectLEZ($b = Null, $fillValue = 0) { callmethod("ArrayMath.selectLEZ", $this->ptr, $b, $fillValue); return $this; }
	function selectEQZ($b = Null, $fillValue = 0) { callmethod("ArrayMath.selectEQZ", $this->ptr, $b, $fillValue); return $this; }
	function selectNEZ($b = Null, $fillValue = 0) { callmethod("ArrayMath.selectNEZ", $this->ptr, $b, $fillValue); return $this; }

    function selectStartOfSecond($majorTickStep = 1, $initialMargin = 0.1) { 
    	callmethod("ArrayMath.selectStartOfSecond", $this->ptr, $majorTickStep, $initialMargin); 
		return $this; 
	}
    function selectStartOfMinute($majorTickStep = 1, $initialMargin = 5) { 
    	callmethod("ArrayMath.selectStartOfMinute", $this->ptr, $majorTickStep, $initialMargin); 
		return $this; 
	}
	function selectStartOfHour($majorTickStep = 1, $initialMargin = 300) {
		callmethod("ArrayMath.selectStartOfHour", $this->ptr, $majorTickStep, $initialMargin);
		return $this; 
	}
	function selectStartOfDay($majorTickStep = 1, $initialMargin = 10800) {
		callmethod("ArrayMath.selectStartOfDay", $this->ptr, $majorTickStep, $initialMargin);
		return $this; 
	}
	function selectStartOfWeek($majorTickStep = 1, $initialMargin = 172800) {
		callmethod("ArrayMath.selectStartOfWeek", $this->ptr, $majorTickStep, $initialMargin);
		return $this; 
	}
	function selectStartOfMonth($majorTickStep = 1, $initialMargin = 432000) {
		callmethod("ArrayMath.selectStartOfMonth", $this->ptr, $majorTickStep, $initialMargin);
		return $this; 
	}
	function selectStartOfYear($majorTickStep = 1, $initialMargin = 5184000) {
		callmethod("ArrayMath.selectStartOfYear", $this->ptr, $majorTickStep, $initialMargin);
		return $this; 
	}
	function selectRegularSpacing($majorTickStep, $minorTickStep = 0, $initialMargin = 0) {
		callmethod("ArrayMath.selectRegularSpacing", $this->ptr, $majorTickStep, $minorTickStep, $initialMargin);
		return $this; 
	}
			
	function trim($startIndex = 0, $len = -1) {
		callmethod("ArrayMath.trim", $this->ptr, $startIndex, $len);
		return $this; 
	}
	function insert($a, $insertPoint = -1) {
		callmethod("ArrayMath.insert", $this->ptr, $a, $insertPoint);
		return $this; 
	}
	function insert2($c, $len, $insertPoint= -1) {
		callmethod("ArrayMath.insert2", $this->ptr, $c, $len, $insertPoint);
		return $this; 
	}
	function replace($a, $b) {
		callmethod("ArrayMath.replace", $this->ptr, $a, $b);
		return $this; 
	}

	function movAvg($interval) {
		callmethod("ArrayMath.movAvg", $this->ptr, $interval);
		return $this; 
	}
	function expAvg($smoothingFactor) {
		callmethod("ArrayMath.expAvg", $this->ptr, $smoothingFactor);
		return $this; 
	}
	function movMed($interval) {
		callmethod("ArrayMath.movMed", $this->ptr, $interval);
		return $this; 
	}
	function movPercentile($interval, $percentile) {
		callmethod("ArrayMath.movPercentile", $this->ptr, $interval, $percentile);
		return $this; 
	}
	function movMax($interval) {
		callmethod("ArrayMath.movMax", $this->ptr, $interval);
		return $this; 
	}
	function movMin($interval) {
		callmethod("ArrayMath.movMin", $this->ptr, $interval);
		return $this; 
	}
	function movStdDev($interval) {
		callmethod("ArrayMath.movStdDev", $this->ptr, $interval);
		return $this; 
	}
	function movCorr($interval, $b = Null) {
		callmethod("ArrayMath.movCorr", $this->ptr, $interval, $b);
		return $this; 
	}
	function lowess($smoothness = 0.25, $iteration = 0) {
		callmethod("ArrayMath.lowess", $this->ptr, $smoothness, $iteration);
		return $this; 
	}
	function lowess2($b, $smoothness = 0.25, $iteration = 0) {
		callmethod("ArrayMath.lowess2", $this->ptr, $b, $smoothness, $iteration);
		return $this; 
	}

	function result() {
		return callmethod("ArrayMath.result", $this->ptr);
	}
	function max() {
		return callmethod("ArrayMath.max", $this->ptr);
	}
	function min() {
		return callmethod("ArrayMath.min", $this->ptr);
	}
	function avg() {
		return callmethod("ArrayMath.avg", $this->ptr);
	}
	function sum() {
		return callmethod("ArrayMath.sum", $this->ptr);
	}
	function stdDev() {
		return callmethod("ArrayMath.stdDev", $this->ptr);
	}
	function med() {
		return callmethod("ArrayMath.med", $this->ptr);
	}
	function percentile($p) {
		return callmethod("ArrayMath.percentile", $this->ptr, $p);
	}
	function maxIndex() {
		return callmethod("ArrayMath.maxIndex", $this->ptr);
	}
	function minIndex() {
		return callmethod("ArrayMath.minIndex", $this->ptr);
	}
	
	function aggregate($srcArray, $aggregateMethod, $param = 50) {
		return callmethod("ArrayMath.aggregate", $this->ptr, $srcArray, $aggregateMethod, $param);
	}
}

class MultiPagePDF extends AutoMethod
{
	function __construct() {
		$this->ptr = callmethod("MultiPagePDF.create");
		autoDestroy($this);
	}
	function __del__() {
		callmethod("MultiPagePDF.destroy", $this->ptr);
	}
	function addPage($c) {
		if (is_subclass_of($c, "BaseChart"))
			$c = $c->makeChart3();
		callmethod("MultiPagePDF.addPage", $this->ptr, $c->ptr);
	}
	function addPage2($d) {
		$this->addPage($d);
	}
}

define("MouseUsageDefault", 0);
define("MouseUsageScroll", 2);
define("MouseUsageZoomIn", 3);
define("MouseUsageZoomOut", 4);

define("DirectionHorizontal", 0);
define("DirectionVertical", 1);
define("DirectionHorizontalVertical", 2);

#[AllowDynamicProperties]
class WebChartImage
{
	function __construct($id) {
		$this->ptr = callmethod("WebChartViewer.create");
		autoDestroy($this);
		$this->putAttrS(":id", $id);
	}
	function __del__() {
		callmethod("WebChartViewer.destroy", $this->ptr);
	}
	
	function getId() { return $this->getAttrS(":id"); }
	
	function setImageUrl($url) { $this->putAttrS(":url", $url); }
	function getImageUrl() { return $this->getAttrS(":url"); }
	
	function setChart($c, $format = PNG) {
		$this->chart = $c;
		$this->outFormat = $format;
		$c->makeChart3();
		$this->setChartMetrics($c->getChartMetrics());
	}
	
	function setChartMetrics($metrics) { $this->putAttrS(":metrics", $metrics); }
	function getChartMetrics() { return $this->getAttrS(":metrics"); }

	function getAttrS($attr, $defaultValue = "") {
		return callmethod("WebChartViewer.getAttrS", $this->ptr, $attr, $defaultValue);
	}
	function getAttrF($attr, $defaultValue = 0) {
		return callmethod("WebChartViewer.getAttrF", $this->ptr, $attr, $defaultValue);
	}
	function putAttrS($attr, $value) {
		callmethod("WebChartViewer.putAttrS", $this->ptr, $attr, $value);
	}
	function putAttrF($attr, $value) {
		callmethod("WebChartViewer.putAttrF", $this->ptr, $attr, $value);
	}
	function getMimeType($img) {
		if (strlen($img) >= 4) {
			$c0 = ord($img[0]);
			$c1 = ord($img[1]);
			$c2 = ord($img[2]);
			$c3 = ord($img[3]);
			if (($c0 == 0x47) && ($c1 == 0x49))
				return "image/gif";
			if (($c1 == 0x50) && ($c2 == 0x4e))
				return "image/png";
			if (($c0 == 0x42) && ($c1 == 0x4d))
				return "image/bmp";
			if (($c0 == 0xff) && ($c1 == 0xd8))
				return "image/jpeg";
			if (($c0 == 0x25) && ($c1 == 0x50) && ($c2 == 0x44) && ($c3 == 0x46))
				return "application/pdf";
			if (($c0 == 0x3c) && ($c1 == 0x3f) && ($c2 == 0x78) && ($c3 == 0x6d))
				return "image/svg+xml";
			if (($c0 == 0x1f) && ($c1 == 0x8b))
				return "image/svg+xml"; 
		}
		return "application/octet-stream";
	}
	function verifyImageUrl() {
		if (isset($this->chart)) {
			$f = $this->outFormat;
			$img = $this->chart->makeChart2(($f == SVGZ) ? SVG : $f);
			$imgUrl = "data:" . $this->getMimeType($img) . ";base64," . base64_encode($img);
			$this->setImageUrl($imgUrl);
		}
	}
	function renderHTML($extraAttrs = null) {
		$this->verifyImageUrl();
		$url = isset($_SERVER["SCRIPT_NAME"]) ? $_SERVER["SCRIPT_NAME"] : "";
		if (isset($_SERVER["PATH_INFO"]))
			$url .= $_SERVER["PATH_INFO"];
		$query = isset($_SERVER["QUERY_STRING"]) ? $_SERVER["QUERY_STRING"] : "";
		return callmethod("WebChartViewer.renderHTML", $this->ptr, $url, $query, $extraAttrs);
	}
}

class WebChartViewer extends WebChartImage
{
	function __construct($id) {
		parent::__construct($id);
		$s = $id."_JsChartViewerState";
		if (isset($_REQUEST[$s]))
			$this->putAttrS(":state", $_REQUEST[$s]);
	}

	function setImageMap($imageMap) { $this->putAttrS(":map", $imageMap); }
	function getImageMap() { return $this->getAttrS(":map"); }
		
	function setChartModel($model) { $this->putAttrS(":model", $model); }
	function getChartModel() { return $this->getAttrS(":model"); }

	function setFullRange($id, $minValue, $maxValue) {
		callmethod("WebChartViewer.setFullRange", $this->ptr, $id, $minValue, $maxValue);
	}
	function getValueAtViewPort($id, $ratio, $isLogScale = false) {
		return callmethod("WebChartViewer.getValueAtViewPort", $this->ptr, $id, $ratio, $isLogScale);
	}
	function getViewPortAtValue($id, $value, $isLogScale = false) {
		return callmethod("WebChartViewer.getViewPortAtValue", $this->ptr, $id, $value, $isLogScale);
	}
	function syncLinearAxisWithViewPort($id, $axis) {
		callmethod("WebChartViewer.syncAxisWithViewPort", $this->ptr, $axis->ptr, $id, 3);
	}	
	function syncLogAxisWithViewPort($id, $axis) {
		callmethod("WebChartViewer.syncAxisWithViewPort", $this->ptr, $axis->ptr, $id, 4);
	}	
	function syncDateAxisWithViewPort($id, $axis) {
		callmethod("WebChartViewer.syncAxisWithViewPort", $this->ptr, $axis->ptr, $id, 5);
	}	

	function makeDelayedMap($imageMap, $compress = 0) {
		return $imageMap;	
	}
	function setContentHeader($img) {
		header("Content-type: " . $this->getMimeType($img));
		if ((strlen($img) >= 2) && ($img[0] == 0x1f) && ($img[1] == 0x8b))
			header("Content-Encoding: gzip");
	}
	function streamChart($img, $filename = null) {
		$this->setContentHeader($img);
		if (!is_null($filename))
			header("Content-Disposition: inline; filename=\"$filename\"");
		print($img);		
	}
	function streamAttachment($img, $filename) {
		$this->setContentHeader($img);
		header("Content-Disposition: attachment; filename=\"$filename\"");
		print($img);		
	}
	function partialUpdateChart($msg = null, $timeout = 0) {
		$this->verifyImageUrl();
		header("Content-type: text/html; charset=utf-8");
		return callmethod("WebChartViewer.partialUpdateChart", $this->ptr, $msg, $timeout);	
	}
	function isPartialUpdateRequest() {	return isset($_REQUEST["cdPartialUpdate"]); }
	function isFullUpdateRequest() {
		if ($this->isPartialUpdateRequest())
			return 0;
		$s = "_JsChartViewerState";
		foreach($_REQUEST as $k => $v) {
			if (substr($k, -strlen($s)) == $s)
				return 1;
		}
		return 0;
	}
	function isStreamRequest() { return isset($_REQUEST["cdDirectStream"]); }
	function isAttachmentRequest() { return isset($_REQUEST["cdAttachment"]); }
	function isViewPortChangedEvent() {	return $this->getAttrF(25, 0) != 0; }
	function getSenderClientId() {
		if ($this->isPartialUpdateRequest())
			return $_REQUEST["cdPartialUpdate"];
		elseif ($this->isStreamRequest())
			return $_REQUEST["cdDirectStream"];
		else
			return null;
	}

	function getViewPortLeft() { return $this->getAttrF(4, 0); }
	function setViewPortLeft($left) { $this->putAttrF(4, $left); }

	function getViewPortRight() { return $this->getViewPortLeft() + $this->getViewPortWidth(); }
		
	function getViewPortTop() { return $this->getAttrF(5, 0); }
	function setViewPortTop($top) { $this->putAttrF(5, $top); }

	function getViewPortBottom() { return $this->getViewPortTop() + $this->getViewPortHeight(); }

	function getViewPortWidth() { return $this->getAttrF(6, 1); }
	function setViewPortWidth($width) { $this->putAttrF(6, $width); }

	function getViewPortHeight() { return $this->getAttrF(7, 1); }
	function setViewPortHeight($height) { $this->putAttrF(7, $height); }

	function getSelectionBorderWidth() { return (int)($this->getAttrF(8, 2)); }
	function setSelectionBorderWidth($lineWidth) { $this->putAttrF(8, $lineWidth); }

	function getSelectionBorderColor() { return $this->getAttrS(9, "Black"); }
	function setSelectionBorderColor($color) { $this->putAttrS(9, $color); }

	function getMouseUsage() { return (int)($this->getAttrF(10, MouseUsageDefault)); }
	function setMouseUsage($usage) { $this->putAttrF(10, $usage); }

	function getScrollDirection() { return (int)($this->getAttrF(11, DirectionHorizontal)); }
	function setScrollDirection($direction) { $this->putAttrF(11, $direction); }

	function getZoomDirection() { return (int)($this->getAttrF(12, DirectionHorizontal)); }
	function setZoomDirection($direction) { $this->putAttrF(12, $direction); }

	function getZoomInRatio() { return $this->getAttrF(13, 2); }
	function setZoomInRatio($ratio) { if ($ratio > 0) $this->putAttrF(13, $ratio); }

	function getZoomOutRatio() { return $this->getAttrF(14, 0.5); }
	function setZoomOutRatio($ratio) { if ($ratio > 0) $this->putAttrF(14, $ratio); }

	function getZoomInWidthLimit() { return $this->getAttrF(15, 0.01); }
	function setZoomInWidthLimit($limit) { $this->putAttrF(15, $limit); }

	function getZoomOutWidthLimit() { return $this->getAttrF(16, 1); }
	function setZoomOutWidthLimit($limit) { $this->putAttrF(16, $limit); }

	function getZoomInHeightLimit() { return $this->getAttrF(17, 0.01); }
	function setZoomInHeightLimit($limit) { $this->putAttrF(17, $limit); }

	function getZoomOutHeightLimit() { return $this->getAttrF(18, 1); }
	function setZoomOutHeightLimit($limit) { $this->putAttrF(18, $limit); }
		
	function getMinimumDrag() { return (int)($this->getAttrF(19, 5)); }
	function setMinimumDrag($offset) { $this->putAttrF(19, $offset); }

	function getZoomInCursor() { return $this->getAttrS(20, ""); }
	function setZoomInCursor($cursor) { $this->putAttrS(20, $cursor); }

	function getZoomOutCursor() { return $this->getAttrS(21, ""); }
	function setZoomOutCursor($cursor) { $this->putAttrS(21, $cursor); }

	function getScrollCursor() { return $this->getAttrS(22, ""); }
	function setScrollCursor($cursor) { $this->putAttrS(22, $cursor); }

	function getNoZoomCursor() { return $this->getAttrS(26, ""); }
	function setNoZoomCursor($cursor) { $this->putAttrS(26, $cursor); }

	function getCustomAttr($key) { return $this->getAttrS($key, ""); }
	function setCustomAttr($key, $value) { $this->putAttrS($key, $value); }
}

class WebViewPortControl extends WebChartImage
{
	function __construct($id) {
		parent::__construct($id);
		$this->putAttrF(":vpc", 1);
	}
}

?>
