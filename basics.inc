<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : basics.inc                                                   //
// Version     : 21.1                                                         //
// Begin       : 2020-07-21                                                   //
// Last Change : 2021-05-25                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  © FeRox Management Consulting GmbH & Co. KG                     //
// -------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify       //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation, either version 3 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// This program is distributed in the hope that it will be useful,            //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <https://www.gnu.org/licenses/>.     //
//                                                                            //
// See ../LICENSE.TXT file for more information.                              //
// -------------------------------------------------------------------------- //
// LICENSING ADDENDUM:                                                        //
// Programs in the SPP (Special Programs) subfolder are coded extensions of   //
// the open source software fx-project. These programs are offered for sale   //
// by the manufacturer FeRox Management Consulting GmbH & Co. KG and require  //
// a valid key for execution. It is forbidden to resell these programs        //
// and/or keys or to pass them on free of charge or use them without the      //
// express written permission of FeRox Management Consulting GmbH & Co. KG.   //
////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * Set all definitions, variables and necessary dynamic paths.
 * Contains all basic functions.
 * Includes all basic subprograms and classes.
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;
set_time_limit(0);

// Block screen display until headers are sent
ob_start();


////////////////////////////////////////////////////////////////////////////////
// DEBUG MODE
////////////////////////////////////////////////////////////////////////////////
// Debug level (bits)
define('FXP_DEBUG_OFF', 0);		// Off
define('FXP_DEBUG_MAX', 1);		// Bit  0 - Maximum: All debug messages     (level 1 or higher)
define('FXP_DEBUG_DEF', 2);		// Bit  1 - Default: Normal debug messages  (level 2 or higher)
define('FXP_DEBUG_MIN', 4);		// Bit  2 - Minimum: Special debug messages (level 3 or higher)
define('FXP_DEBUG_FIL', 8);		// Bit  3 - Names of (optional) included files
define('FXP_DEBUG_FCT', 16);	// Bit  4 - All function calls
define('FXP_DEBUG_PST', 32);	// Bit  5 - Superglobals: POST + GET
define('FXP_DEBUG_TIM', 64);	// Bit  6 - Runtimes
define('FXP_DEBUG_MEM', 128);	// Bit  7 - Memory consumption
define('FXP_DEBUG_INF', 256);	// Bit  8 - fx-project special: Global runtime + memory
define('FXP_DEBUG_FRM', 512);	// Bit  9 - fx-project special: Framework routines
define('FXP_DEBUG_MST', 1024);	// Bit 10 - fx-project special: Mask statuses
define('FXP_DEBUG_VAL', 2048);	// Bit 11 - fx-project special: Default validation routines
define('FXP_DEBUG_ALL', FXP_DEBUG_MAX|FXP_DEBUG_FIL|FXP_DEBUG_FCT|FXP_DEBUG_PST|FXP_DEBUG_TIM|FXP_DEBUG_MEM|FXP_DEBUG_INF|FXP_DEBUG_FRM|FXP_DEBUG_MST|FXP_DEBUG_VAL);	// All debug messages
// Array that contains as keys the program names to be debugged and as value the debug level and/or special debug modes
if(!isset($GLOBALS['__debug']) || !is_array($GLOBALS['__debug']))
	$GLOBALS['__debug']=array();
if(!isset($GLOBALS['__debug']['debugmode']))
	$GLOBALS['__debug']['__DBG']=0;
if(!isset($GLOBALS['__DGBLVL']))
	$GLOBALS['__DGBLVL']=0;
// Debug this file? (Uncomment the next line)
//$GLOBALS['__debug'][__FILE__]=1;


////////////////////////////////////////////////////////////////////////////////
// DEFINITIONS
////////////////////////////////////////////////////////////////////////////////
// ...Version (= max version from subfiles)
define('FXP_VERSION', '21.1');
// ...Charset/Locale
define('FXP_CHARSET', 'UTF-8');
define('FXP_LOCALE', 'de_DE.UTF-8');
// ...Action modes
define('FXP_DISPLAY', 1);
define('FXP_CREATE', 2);
define('FXP_CHANGE', 3);
define('FXP_DELETE', 4);
// ...Newline for email class
define('CRLF', $GLOBALS['nl_b']);
// ...User statuses
define('FXP_USER_NEW', 0);
define('FXP_USER_ACTIVE', 1);
define('FXP_USER_INACTIVE', 2);
define('FXP_USER_LOCKED', 3);
// ...Text reference types
define('FXP_TRT_TABLE', 1);
define('FXP_TRT_MASK', 2);
define('FXP_TRT_FIELD', 3);
define('FXP_TRT_UNUSED_4', 4);
define('FXP_TRT_UNUSED_5', 5);
define('FXP_TRT_PROGRAM', 6);
// Person types (Value from table "references", field 30)
define('FXP_PT_CLIENT', 274);				// Client
define('FXP_PT_CLIENT_CP', 2090);			// Client's contact person
define('FXP_PT_CUSTOMER', 276);				// Customer
define('FXP_PT_CUSTOMER_CP', 620);			// Customer's contact person
define('FXP_PT_PARTNER', 2091);				// Contract partner
define('FXP_PT_PARTNER_CP', 2092);			// Contract partner's contact person
define('FXP_PT_EMPLOYEE', 275);				// Employee (internal)
define('FXP_PT_CONTRACTOR', 840);			// Contractor (external)
define('FXP_PT_CONTACT', 1372);				// Contact
define('FXP_PT_CONTACT_CP', 841);			// Contact's contact person
// Gender types
define('FXP_GT_MALE', 8);					// male
define('FXP_GT_FEMALE', 9);					// female
define('FXP_GT_DIVERSE', 2802);				// diverse
// Project types (Value from table "category", mastercategory 57)
define('FXP_PRJ_MAIN', 59);					// Main project / Mother project
define('FXP_PRJ_SUB', 60);					// Sub-project
define('FXP_PRJ_TASK', 61);					// Task
// Project status (Value from table "references", field 137)
define('FXP_PS_PLANNED', 297);
define('FXP_PS_INACTIVE', 299);
define('FXP_PS_ACTIVE', 300);
define('FXP_PS_COMPLETED', 301);
// Time entries (Value from table "references", field 336)
define('FXP_TE_TIMEREC', 101);				// Time recording entry
define('FXP_TE_PROJECT', 102);				// Project time data entry
define('FXP_TE_ABSENCES', 103);				// Absence/Missing days entry
define('FXP_TE_CARRY_MONTH', 104);			// Time recording: Monthly calculated carryover entry
define('FXP_TE_CARRY_START', 105);			// Time recording: Start carryover entry
define('FXP_TE_TIMEREC_SUM', 110);			// Time recording sum entry
define('FXP_TE_MATUSAGE', 1995);			// Material usage recording entry
define('FXP_TE_TRAVELEXP', 2225);			// Travel expanses entry
define('FXP_TE_TRAVELEXP_TIMEREC', 2273);	// Travel expanses entry with time recording
// Time units (Value from table "references", field 330)
define('FXP_TU_HOUR', 484);					// Hour
define('FXP_TU_PERSDAY', 485);				// Person day / Man day
define('FXP_TU_PERSWEEK', 923);				// Person week / Man week
define('FXP_TU_PERSMONTH', 486);			// Person month / Man month
define('FXP_TU_PERSYEAR', 487);				// Person year / Man year
// Appointment/Absent types (Value from table "references", field 434)
define('FXP_AT_REMINDER', 180);
define('FXP_AT_INVITATION', 187);
define('FXP_AT_MILESTONE', 375);
define('FXP_AT_MEETING', 661);
define('FXP_AT_CONFERENCE', 662);
define('FXP_AT_BUSINESSDINNER', 718);
define('FXP_AT_CONTACT', 719);
define('FXP_AT_MESSAGE', 971);
define('FXP_AT_PROJECTMESSAGE', 1131);
define('FXP_AT_PRIVATE', 720);
define('FXP_AT_TODO', 976);
define('FXP_AT_VACATIONREQUEST', 1178);
define('FXP_AT_VACATION', 1039);
define('FXP_AT_SPECIALVACATIONREQUEST', 2877);
define('FXP_AT_SPECIALVACATION', 2878);
define('FXP_AT_TRAININGREQUEST', 2106);
define('FXP_AT_TRAINING', 1468);
define('FXP_AT_SICKCALL', 2105);
define('FXP_AT_SICK', 1040);
define('FXP_AT_SLIDINGDAYREQUEST', 1940);
define('FXP_AT_SLIDINGDAY', 1941);
define('FXP_AT_OVERTIMEOUTPAYREQUEST', 1942);
define('FXP_AT_OVERTIMEOUTPAY', 1943);
define('FXP_AT_ABSENT', 2879);
define('FXP_AT_VARIOUS', 2297);
// Appointment/Absent categories
define('FXP_AC_VARIOUS', 0);
define('FXP_AC_PRIVATE', 1);
define('FXP_AC_BUSINESS', 2);
define('FXP_AC_ABSENT', 3);
// Holiday lengths (Value from table "references", field 1167)
define('FXP_HL_FULL', 2174);		// Full Day
define('FXP_HL_HALF', 2175);		// Half Day
define('FXP_HL_DISPLAY', 2176);		// Display Only
// Bit mask for appointment roles
define('FXP_BAR_SYSADMIN', 128);
define('FXP_BAR_EXECUTIVE', 64);
define('FXP_BAR_MANAGEMENT', 32);
define('FXP_BAR_PROJECTLEAD', 16);
define('FXP_BAR_DEPARTMENTLEAD', 4);
define('FXP_BAR_SUPERVISOR', 2);
define('FXP_BAR_SELF', 1);
// Invoice types (Value from table "references", field 69)
define('FXP_INV_FIX', 483);		// Fixed Price
define('FXP_INV_HOUR', 639);	// On a per hour basis
define('FXP_INV_MAT_HU', 645);	// On a per hour or unit basis
define('FXP_INV_DAYRATE', 646);	// On a daily rate basis ---< not yet implemented >---
define('FXP_INV_NOT', 640);		// Not invoiceable
// Chartdirector display types
define('FXP_CT_LINE', 0);
define('FXP_CT_AREA', 1);
define('NoValue', 1.7E+308);
// Scheduler program types (Value from table "references", field 1209)
define('FXP_SDL_EMAIL', 2693);		// Send emails
define('FXP_SDL_WARNING', 2695);	// Check project warnings
define('FXP_SDL_TIMEREC', 2166);	// Check time recordings

////////////////////////////////////////////////////////////////////////////////
// OS SETTINGS + LOCALE + CHARSET
////////////////////////////////////////////////////////////////////////////////
fxSetOS();
setlocale(LC_CTYPE, FXP_LOCALE);
$ini_charset=strtoupper(ini_get('default_charset'));
$fxp_charset=strtoupper(FXP_CHARSET);
if($ini_charset != $fxp_charset)
	ini_set('default_charset', $fxp_charset);

////////////////////////////////////////////////////////////////////////////////
// VARIABLES
////////////////////////////////////////////////////////////////////////////////
// ...Current date and time in format YYYYMMDDhhmmss
$GLOBALS['datetime']=fxNow();
$GLOBALS['date']=substr($GLOBALS['datetime'],0,8);
// ...Error and/or success handling
$GLOBALS['err']=false;		// Global error variable
$GLOBALS['ema']=array();	// Global error message array with keys, titles + messages
// ...Separators used in text files when returning via AJAX to javascript
$GLOBALS['_divstr']=array('!-0-!', '!-1-!', '!-2-!');
// ...Array of all fx-project subpaths
$GLOBALS['_subpath_array']=array();
// ...Min. program type and year
$GLOBALS['_min_pf_type']=10;	// Min. for each user is program function type 10 (= User program)
$GLOBALS['_min_year']=1900;		// Min. valid year

// ...Field types
$GLOBALS['_ftypes'] = array(
	1=>'text',			//  1 = Text
	'autoinkrement',	//  2 = Serial
	'memo',				//  3 = Memo
	'ja/nein',			//  4 = Bit
	'ganzzahl',			//  5 = Integer
	'dezimal',			//  6 = Decimal
	'datum',			//  7 = Date
	'userformat',		//  8 = User Format
	'zeit',				//  9 = Time
	'datetime',			// 10 = Date + Time
	'betrag',			// 11 = Amount
	'ganzzahllang',		// 12 = Long Integer
	'systemvariable',	// 13 = System
	'zeitspanne',		// 14 = Timespan
	'text4000',			// 15 = Long Text
	'budget_int',		// 16 = Internal Budget
	'budget_ext',		// 17 = External Budget
	99=>'ganzzahl_tz'	// 99 = Integer (with Thousand Separator)
);

// ...Database types
$GLOBALS['_dbtype']='';
$GLOBALS['_dbtypes']=array(
	// 1 = PostgreSQL
	1=>array(
		'type'=>'pgsql',
		'text'=>'PostgreSQL', 'alt1'=>'Postgres',
		// Field types in database
		'ftypes'=>array(
			1=>'VARCHAR',			//  1 = Text
			'SERIAL',				//  2 = Serial
			'TEXT',					//  3 = Memo
			'INT2',					//  4 = Bit
			'INT8',					//  5 = Integer
			'DECIMAL(22,4)',		//  6 = Decimal
			'VARCHAR(14)',			//  7 = Date
			'VARCHAR',				//  8 = User Format
			'VARCHAR(14)',			//  9 = Time
			'VARCHAR(14)',			// 10 = Date + Time
			'DECIMAL(22,4)',		// 11 = Amount
			'INT8',					// 12 = Long Integer
			'VARCHAR',				// 13 = System
			'INT8',					// 14 = Timespan
			'TEXT',					// 15 = Long Text
			'DECIMAL(22,4)',		// 16 = Internal Budget
			'DECIMAL(22,4)',		// 17 = External Budget
			99=>'INT8'				// 99 = Integer (with Thousand Separator)
		)
	),
	// 2 = MS SQLServer
	2=>array(
		'type'=>'sqlsrv',
		'text'=>'SQLServer', 'alt1'=>'MSSQL', 'alt2'=>'MSSQLServer',
		// Field types in database
		'ftypes'=>array(
			1=>'VARCHAR',			//  1 = Text
			'INT IDENTITY(1,1)',	//  2 = Serial
			'TEXT',					//  3 = Memo
			'INT',					//  4 = Bit
			'INT',					//  5 = Integer
			'DECIMAL(22,4)',		//  6 = Decimal
			'VARCHAR(14)',			//  7 = Date
			'VARCHAR',				//  8 = User Format
			'VARCHAR(14)',			//  9 = Time
			'VARCHAR(14)',			// 10 = Date + Time
			'DECIMAL(22,4)',		// 11 = Amount
			'INT',					// 12 = Long Integer
			'VARCHAR',				// 13 = System
			'INT',					// 14 = Timespan
			'TEXT',					// 15 = Long Text
			'DECIMAL(22,4)',		// 16 = Internal Budget
			'DECIMAL(22,4)',		// 17 = External Budget
			99=>'INT'				// 99 = Integer (with Thousand Separator)
		)
	)
);


////////////////////////////////////////////////////////////////////////////////
// INCLUDER
////////////////////////////////////////////////////////////////////////////////
// Array that contains as keys the program names to be included by the includer file.
// This array will be filled by each program with their requirements.
if(!isset($GLOBALS['__includer']) || !is_array($GLOBALS['__includer']))
	$GLOBALS['__includer']=array();

// ...Globals
$GLOBALS['fxpglobals']=array();
$GLOBALS['fxptdata']=array();
$GLOBALS['fxpselects']=array();
$GLOBALS['fxpvars']=array('trrights'=>array(),'messages'=>array(),'msgarr'=>array(),'ibuffer'=>array(),'bbuffer'=>array());
$GLOBALS['_maskcounter']=0;
$GLOBALS['_fieldcounter']=0;

$__dagpia=array('no_headers', 'inapp', 'insync');
foreach($__dagpia as $__dagpi)
{
	if(isset($GLOBALS[$__dagpi]))
	{
		if(fxIsArray($_GET) && isset($_GET[$__dagpi]))
		{
			unset($_GET[$__dagpi]);
			if(isset($GLOBALS[$__dagpi]))
				unset($GLOBALS[$__dagpi]);
		}
		if(fxIsArray($_POST) && isset($_POST[$__dagpi]))
		{
			unset($_POST[$__dagpi]);
			if(isset($GLOBALS[$__dagpi]))
				unset($GLOBALS[$__dagpi]);
		}
	}
}

// ...Appointment category array
$_tca=array(
	FXP_AC_PRIVATE	=> FXP_AT_PRIVATE,
	FXP_AC_BUSINESS	=> FXP_AT_MEETING.', '.FXP_AT_CONFERENCE.', '.FXP_AT_BUSINESSDINNER.', '.FXP_AT_CONTACT.', '.FXP_AT_MESSAGE.', '.FXP_AT_TODO.', '.FXP_AT_PROJECTMESSAGE,
	FXP_AC_ABSENT	=> FXP_AT_ABSENT.', '.FXP_AT_VACATION.', '.FXP_AT_VACATIONREQUEST.', '.FXP_AT_SPECIALVACATION.', '.FXP_AT_SPECIALVACATIONREQUEST.', '.FXP_AT_SICK.', '.FXP_AT_SICKCALL.', '.FXP_AT_TRAINING.', '.FXP_AT_TRAININGREQUEST.', '.FXP_AT_SLIDINGDAY.', '.FXP_AT_SLIDINGDAYREQUEST,
	FXP_AC_VARIOUS	=> FXP_AT_REMINDER.', '.FXP_AT_INVITATION.', '.FXP_AT_VARIOUS
);


////////////////////////////////////////////////////////////////////////////////
// DYNAMIC PATHS
////////////////////////////////////////////////////////////////////////////////
fxSetDynamicPaths();


////////////////////////////////////////////////////////////////////////////////
// SESSION ENVIRONMENT
////////////////////////////////////////////////////////////////////////////////
fxSetSession();
if($GLOBALS['locseskey'] === 'new')
{
	@header('HTTP/1.1 301 Moved Permanently'); 
	@header('location: '.$GLOBALS['__server_array']['url'].'index.php');
	die;
}


////////////////////////////////////////////////////////////////////////////////
// HTML HEADERS
////////////////////////////////////////////////////////////////////////////////
if(!isset($GLOBALS['no_headers']))
{
	@header('Content-type: text/html; charset='.FXP_CHARSET);
	// Send additional headers?
	if(fxIsArray($GLOBALS['__headers']))
	{
		foreach($GLOBALS['__headers'] as $htext)
			@header($htext);
	}
}

// Allow screen display again
$_pre_content=ob_get_contents();
ob_end_clean();
if(strlen($_pre_content))
	echo($_pre_content);


////////////////////////////////////////////////////////////////////////////////
// SESSION
////////////////////////////////////////////////////////////////////////////////
if(!strlen($GLOBALS['locstoid']) || !strlen($GLOBALS['sesstoid']) || !strlen($GLOBALS['locseskey']) || !$GLOBALS['locseschecked'])
{
	echo('<!DOCTYPE html>');
	echo('<html style="width:100%;height:100%;font-family:verdana,arial,helvetica,sans-serif; font-size:12px;">');
	echo('<head><title>fx-project: JavaScript Checker</title></head>');
	echo('<body style="width:100%;height:100%;margin:0;padding:0;">');

	echo('<div style="position:absolute;left:0;top:46%;width:100%;">');
	if($GLOBALS['locsescnt'] < 10)	// Max. 10 tries to get id's to prevent endless loop
	{
		echo('<div style="color:#444;font-size:larger;text-align:center;"><b>fx-project:</b> Starting local session &hellip;</div><div style="padding-top:12px;color:#aaa;font-size:smaller;text-align:center;">(If it isn\'t reloading, JavaScript is probalby not activated!)</div>');

		echo('<form id="fxform" action="'.$GLOBALS['__server_array']['urlr'].'" method="post"><input id="locstoid" name="locstoid" type="hidden" value=""><input id="sesstoid" name="sesstoid" type="hidden" value=""><input id="locseskey" name="locseskey" type="hidden" value="'.$GLOBALS['locseskey'].'"><input id="locsescnt" name="locsescnt" type="hidden" value="'.($GLOBALS['locsescnt']+1).'"><input id="emode" name="emode" type="hidden" value="'.$GLOBALS['emode'].'"><input id="lts" name="lts" type="hidden" value="'.$GLOBALS['lts'].'"></form>');
		echo('<script type="text/javascript">');
		echo('locstoid=localStorage.getItem(\'locstoid\'); if(!locstoid || (locstoid.length != 15)) { locstoid=\'L'.date('YmdHis').'\'; localStorage.setItem(\'locstoid\', locstoid);} ');
		echo('sesstoid=sessionStorage.getItem(\'sesstoid\'); if(!sesstoid || (sesstoid.length != 18)) { sesstoid=\'S'.date('YmdHis').substr(str_replace('.','', microtime(true)),-3).'\'; sessionStorage.setItem(\'sesstoid\', sesstoid);} ');
//		if(!$GLOBALS['locsescnt']) echo('alert(\'New Local/Session storage: locsescnt='.$GLOBALS['locsescnt'].', locstoid=\'+locstoid+\', sesstoid=\'+sesstoid+\'\\nemode: \'+document.getElementById(\'emode\').value); ');
		echo('document.getElementById(\'locstoid\').value=locstoid; document.getElementById(\'sesstoid\').value=sesstoid; document.getElementById(\'fxform\').submit();');
		echo('</script>');
	}
	else
		echo('<div style="color:#e1001a;font-size:larger;text-align:center;"><b>ERROR:</b> Could not get local/session storage id!</div><div style="padding-top:12px;color:#aaa;font-size:smaller;text-align:center;">(JavaScript is activated, but Browser probably doesn\'t support HTML5!)</div>');
	echo('</div>');

	echo('</body>');
	echo('</html>');

	die;
}
if(isset($GLOBALS['insync']))
{
	$GLOBALS['locstoid']='L00000000000000';
	$GLOBALS['sesstoid']='S00000000000000-'.$GLOBALS['insync'];
}
$GLOBALS['fxpglobals']['locstoid']=$GLOBALS['locstoid'];
$GLOBALS['fxpglobals']['sesstoid']=$GLOBALS['sesstoid'];
fxDebug(array('locstoid'=>$GLOBALS['locstoid'], 'sesstoid'=>$GLOBALS['sesstoid'], 'lts'=>$GLOBALS['lts']),'Local/Session storage id\'s + Login timestamp', 2);

// Login counter
if(!isset($GLOBALS['lcnt']))
{
	if(fxIsArray($_GET) && isset($_GET['lcnt']))
		$GLOBALS['lcnt']=(int)$_GET['lcnt'];
	else
		$GLOBALS['lcnt']=0;
}
fxDebug($GLOBALS['lcnt'],'$GLOBALS[\'lcnt\']',1);

// Load session globals
fxSession();
fxDebug($GLOBALS['fxpglobals'],'$GLOBALS[\'fxpglobals\']',2);

////////////////////////////////////////////////////////////////////////////////
// REQUIREMENTS
////////////////////////////////////////////////////////////////////////////////
if(!isset($GLOBALS['nosession']) || !$GLOBALS['nosession'])
{
	// ...Database
	$GLOBALS['__includer']['db']=true;
	// ...Date and time functions
	$GLOBALS['__includer']['datetime']=true;
	$GLOBALS['__includer']['calendar']=true;
	// ...Various function collections
	$GLOBALS['__includer']['tools1']=true;
	$GLOBALS['__includer']['tools2']=true;
	$GLOBALS['__includer']['tools3']=true;
	$GLOBALS['__includer']['tools4']=true;
	$GLOBALS['__includer']['tools5']=true;
	// ...Extended block function collection
	$GLOBALS['__includer']['tools_spp']=true;
	// ...Various special function collections
	$GLOBALS['__includer']['tools_pers']=true;
	$GLOBALS['__includer']['tools_prj']=true;
	// ...Mask
	$GLOBALS['__includer']['mask']=true;
	// ...Select fields
	$GLOBALS['__includer']['select']=true;

	require('includer.inc');
}

// fx-project type + version (without trailing 0's)
fxSetVersion();

// Allow external links?
$author_string='FeRox Management Consulting GmbH & Co. KG';
$copyright_string='&copy; '.$author_string;
$download_string='https://www.fx-project.org';
if(!fxIsArray($GLOBALS['fxpglobals']) || !fxIsArray($GLOBALS['fxpglobals']['dbparam']) || !isset($GLOBALS['fxpglobals']['dbparam']['elinks']) || !$GLOBALS['fxpglobals']['dbparam']['elinks'])
{
	$_elinks=false;
	$copyright_link=$copyright_string;
	$download_link='<span class="blue normal">'.$download_string.'</span>';
}
else
{
	$_elinks=true;
	$copyright_link='<a class="fxlink" href="https://www.ferox.de" target="_blank">'.$copyright_string.'</a>';
	$download_link='<a class="fxlink" href="https://www.fx-project.org" target="_blank">'.$download_string.'</a>';
}

// Determine which browser?
$browser_text=fxf_detBrowser();
fxDebug($browser_text, $GLOBALS['fxpglobals']['browser'].': $browser_text', 1);

// Set user menu?
if(fxIsArray($_POST) && isset($_POST['set_user_menu']))
	$GLOBALS['fxpglobals']['umenu']=$_POST['set_user_menu'];

// CSS color info array
if(!fxIsArray($GLOBALS['cstyle']))
	fxpGetCSSColors();


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Debug a variable, i.e. show the content of a variable - especially useful for arrays
 *
 * @param various $varname   - Mandatory parameter:                Variable that should be debugged
 * @param string  $dbgheader - Optional parameter (default = ''):  Header to be displayed
 * @param integer $level     - Optional parameter (default = 1):   Level of debug
 */
function fxDebug($varname, $dbgheader='', $level=1)
{
	// Disable debug completely, uncomment the next line
	// return;

	// Debug informations
	$dbga=debug_backtrace(); $dbgf=$dbga[0]['file']; $dbgl=$dbga[0]['line'];

	// Special handling for Superglobals
	if(($varname === '_POST') || ($varname === '_GET') || ($varname === '_FILES'))
	{
		$dbgheader=$varname;
		$varname=$GLOBALS[$varname];
		$level=0;
	}

	// Check if debug is turned on for this file or globally
	$dbg=0;
	if(isset($GLOBALS['__debug']) && is_array($GLOBALS['__debug']))
	{
		// ...turned on for this
		if(isset($GLOBALS['__debug'][$dbgf]) && $GLOBALS['__debug'][$dbgf])
			$dbg=(int)$GLOBALS['__debug'][$dbgf];
		// ...turned on globally
		else if(isset($GLOBALS['__debug']['_GDL']) && $GLOBALS['__debug']['_GDL'])
			$dbg=(int)$GLOBALS['__debug']['_GDL'];
	}
	// ...Turned on debug level greater than call level -> leave debug function
	//    (Hint: Set call level to 0 if debug should be displayed regardless of the turned on debug level)
	if(($level > 0) && (!$dbg || ($dbg > $level)))
		return;

	if(@isset($GLOBALS[$varname]))
	{
		$variablenname=$varname;
		$varname=$GLOBALS[$varname];
	}
	$dbgheader=trim($dbgheader);

	// Display
	echo('<div style="position:relative;width:99%;margin-bottom:6px;background:#fff;border-top-left-radius:12px;border-top-right-radius:12px;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">'.$GLOBALS['nl']);
	// ...Headline
	echo('	<div style="position:relative;padding:4px 8px;padding-right:200px;background:#006b9f;color:#fff;font-size:1.5em;font-weight:bolder;border-top-left-radius:12px;border-top-right-radius:12px;">'.$GLOBALS['nl']);
	if(strlen($dbgheader))
		echo('		'.strtr(fxHtmlEncode(strtr($dbgheader, array('<br>'=>"#hr#", '<br />'=>"#hr#", "\r\n"=>"#br#", "\n"=>"#br#"))), array('#hr#'=>"<hr size=1 color=white />", '#br#'=>"<br />")).$GLOBALS['nl']);
	else if(isset($variablenname) && strlen($variablenname))
		echo('		Content of variable <b>$'.$variablenname.'</b> <i>(Type: '.gettype($varname).')</i>'.$GLOBALS['nl']);
	else
		echo('		Content of variable <i>(Type: '.gettype($varname).')</i>'.$GLOBALS['nl']);
	echo('		<div style="position:absolute;bottom:4px;right:8px;color:#000;font-size:0.5em;">&quot;'.$dbgf.'&quot; (line '.$dbgl.')</div>'.$GLOBALS['nl']);
	echo('	</div>'.$GLOBALS['nl']);
	// ...Content
	echo('	<div style="position:relative;width:100%;padding:0;background:#444;">'.$GLOBALS['nl']);
	echo('		<table width=100% border=0 cellpadding=3 cellspacing=1">'.$GLOBALS['nl']);
	fxDebugContent($varname);
	echo('		</table>'.$GLOBALS['nl']);
	echo('	</div>'.$GLOBALS['nl']);
	echo('</div>'.$GLOBALS['nl']);
}

/**
 * 2nd part of the debug a variable function that calls itself again for arrays
 *
 * @param various $varcontent - Mandatory parameter:                Variable that should be debugged
 * @param string  $varkey     - Optional parameter (default = ''):  Array key to be displayed
 */
function fxDebugContent($varcontent, $varkey='')
{
	$wt=64;
	// Array: Cycle recursively through all keys
	if(is_array($varcontent) && sizeof($varcontent))
	{
		echo('		<tr>'.$GLOBALS['nl']);
		echo('			<td valign=top nowrap style="width:'.$wt.'px;background:#ccc;"><span style="color:#006b9f;">'.gettype($varcontent).'</span></td>'.$GLOBALS['nl']);
		echo('			<td valign=top nowrap style="background:#ddd;color:#005077;">'.str_repeat('&nbsp;',2*max(0, substr_count($varkey,'[')-1)).$varkey.'</td>'.$GLOBALS['nl']);
		echo('			<td valign=top nowrap style="background:#eee;"><i style="color:#888;">Size: '.sizeof($varcontent).'</i></td>'.$GLOBALS['nl']);
		echo('		</tr>'.$GLOBALS['nl']);
		foreach($varcontent as $vkey => $vvalue)
		{
			$ka="";
			if(gettype($vkey) == 'string')
				$ka="'";
			fxDebugContent($vvalue, $varkey.'['.$ka.'<b>'.$vkey.'</b>'.$ka.']');
		}
		return;
	}

	echo('		<tr>'.$GLOBALS['nl']);
	// ...Display type
	$type=gettype($varcontent);
	if($type == 'resource')
		$type .= ': '.get_resource_type($varcontent);
	echo('			<td nowrap valign=top style="width:'.$wt.'px;background:#ddd;"><span style="color:#006b9f;">'.$type.'</span></td>'.$GLOBALS['nl']);
	// ...Recursive call from an array key?
	if(strlen($varkey))
	{
		$lsp=strrpos($varkey,'[');
		$varkey=substr($varkey,0,$lsp).'<span style="color:#000;">'.substr($varkey,$lsp).'</span>';
		echo('			<td valign=top nowrap style="background:#eee;color:#005077;">'.str_repeat('&nbsp;',2*max(0, substr_count($varkey,'[')-1)).$varkey.'</td>'.$GLOBALS['nl']);
		echo('			<td width=100% valign=top style="background:#fff;white-space:pre-wrap;word-wrap:break-word;">');
	}
	else
		echo('			<td width=100% colspan=2 valign=top style="background:#fff;white-space:pre-wrap;word-wrap:break-word;">');
	// ...Type: Array
	if(is_array($varcontent))
		echo('<i style="color:#666;">(empty)</i>');
	// ...Type: Boolean
	else if(gettype($varcontent) == 'boolean')
	{
		if($varcontent)
			echo('<span class="dbg" style="color:#009f6b;">true</span>');
		else
			echo('<span class="dbg" style="color:#e1001a;">false</span>');
	}
	// ...Type: Resource or Object
	else if((gettype($varcontent) == 'resource') || (gettype($varcontent) == 'object'))
		echo('<span class="dbg" style="006b9f;">'.fxHtmlEncode(print_r($varcontent,true)).'</span>');
	// ...Defined variable
	else if(isset($varcontent))
		echo('<span class="dbg" style="006b9f;">'.fxHtmlEncode($varcontent).'</span>');
	// ...Undefined variable
	else
		echo('<i style="color:#e1001a;">(undefined)</i>');
	echo('</td>'.$GLOBALS['nl']);
	echo('		</tr>'.$GLOBALS['nl']);
}

/**
 * Convert all html characters to printable charaters, i.e. also display tags
 *
 * @param string $h   - Mandatory parameter:                Html text
 * @param string $enc - Optional parameter (default = ''):  Encoding charset, if empty the default php charset will be used
 *
 * @return Converted html text
 */
function fxHtmlEncode($h, $enc='')
{
	if(!strlen($enc))
		$enc=ini_get('default_charset');

	return htmlentities($h,ENT_QUOTES,$enc);
}

/**
 * Check if a variable is an array that is not empty or has an explicit key
 *
 * @param various $arr - Mandatory parameter:                Variable to be checked
 * @param string  $k   - Optional parameter (default = ''):  Check for this explicit key
 *
 * @return true or false
 */
function fxIsArray($arr, $k='')
{
	if(is_array($arr) && sizeof($arr) && (!strlen($k) || isset($arr[$k])))
		return true;
	return false;
}

/**
 * Helper debug function for comparing to values (Value 1 and 2)
 *
 * @param various $v1 - Mandantory parameter:    Value 1
 * @param various $v2 - Mandantory parameter:    Value 2
 * @param string  $n1 - Optional parameter (default = ''):  Variable name of Value 1 (just for display purposes)
 * @param string  $n2 - Optional parameter (default = ''):  Variable name of Value 2 (just for display purposes)
 */
function fxf_compareDebug($v1, $v2, $n1='', $n2='')
{
	// Value 1
	$h='<b>$';
	if(strlen($n1))
		$h .= $n1;
	else
		$h .= 'v1';
	$h .= '</b>: <span class=lightgrey>[</span><span class=blue>'.fxHtmlEncode($v1).'</span><span class=lightgrey>]</span> <span class=lightblue>('.gettype($v1);
	if(is_string($v1))
	{
		$h .= ', strlen='.strlen($v1);
		if(function_exists('mb_strlen'))
			$h .= ', mb_strlen='.mb_strlen($v1);
	}
	$h .= ')</span>';

	// Value 2
	$h .= ', <b>$';
	if(strlen($n2))
		$h .= $n2;
	else
		$h .= 'v2';
	$h .= '</b>: <span class=lightgrey>[</span><span class=blue>'.fxHtmlEncode($v2).'</span><span class=lightgrey>]</span> <span class=lightblue>('.gettype($v2);
	if(is_string($v2))
	{
		$h .= ', strlen='.strlen($v2);
		if(function_exists('mb_strlen'))
			$h .= ', mb_strlen='.mb_strlen($v2);
	}
	$h .= ')</span>';

	// Result
	$h .= '<br />[==] &rarr; ';
	if($v1 == $v2)
		$h .= '<b class=green>true</b>';
	else
		$h .= '<b class=red>false</b>';
	$h .= ', [===] &rarr; ';
	if($v1 === $v2)
		$h .= '<b class=green>true</b>';
	else
		$h .= '<b class=red>false</b>';
	$h .= '<hr />';

	echo($h);
}

/**
 * Helper debug function for calling functions
 */
function fxDebugFunction()
{
	$_dbbta=debug_backtrace();
	if(is_array($_dbbta) && (sizeof($_dbbta) > 1))
	{
//if($_dbbta[1]['function'] == 'fxpdf_page') fxDebug($_dbbta, 'fxDebugFunction: $_dbbta', 0);
		$_args='(&nbsp;';
		if(is_array($_dbbta[1]['args']) && sizeof($_dbbta[1]['args']))
		{
			foreach($_dbbta[1]['args'] as $_argc => $_arg)
			{
				if($_argc)
					$_args .= '&nbsp;<span class="lightergrey">,</span>&nbsp;&nbsp;';

				if(is_object($_arg))
					$_args .= '<i class="green">{object}</i>';
				else
				{
					if(is_resource($_arg))
						$_args .= '<b class="green">'.$_arg.'</b>';
					else if(is_bool($_arg))
					{
						if($_arg)
							$_args .= '<b class="green">true</b>';
						else
							$_args .= '<b class="red">false</b>';
					}
					else if(is_string($_arg))
						$_args .= '\'<b>'.$_arg.'\'</b>';
					else
						$_args .= '<b>'.$_arg.'</b>';
					$_args .= ' <span class="lightergrey">(type='.gettype($_arg).')</span>';
				}
			}
		}
		$_args .= '&nbsp;)';

		$ds  = '<div class="dbgdl" style="position:relative;width:100%;height:auto;text-align:top;align:top;">';
		$ds .= '<div style="position:relative;display:inline-block;top:0;margin-left:2px;"><img src="'.$GLOBALS['gfxpath'].'tol_b_16x16.png"></div>';
		$ds .= '<div style="position:relative;display:inline-block;top:0;margin-left:8px;width:200px;font-weight:bold;">'.$_dbbta[1]['function'].'</div>';
		$ds .= '<div class="blue" style="position:relative;display:inline-block;top:0;width:800px;">'.$_args.'</div>';
		$ds .= '<div class="lightergrey" style="position:relative;display:inline-block;top:0;font-size:smaller;">'.$_dbbta[1]['file'].' ('.$_dbbta[1]['line'].')</div>';
		$ds .= '<div class="grey" style="position:relative;display:inline-block;top:0;font-size:smaller;">&nbsp;&rarr;&nbsp;'.$_dbbta[0]['file'].' ('.$_dbbta[0]['line'].')</div>';
		$ds .= '</div>'.$GLOBALS['nl'];

		echo($ds);
	}
}

/**
 * Set and determine all OS relative things like newlines, file separators and server variables
 */
function fxSetOS()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Determine current OS (Operating System): Windows or Linux
	$GLOBALS['_os']='windows';
	if((isset($_SERVER) && (isset($_SERVER['PATH']) && (substr($_SERVER['PATH'], 0,1) == '/') || (isset($_SERVER['Path']) && substr($_SERVER['Path'], 0,1) == '/') || (isset($_SERVER['path']) && substr($_SERVER['path'], 0,1) == '/'))) || (isset($_ENV) && (isset($_ENV['PATH']) && (substr($_ENV['PATH'], 0,1) == '/') || (isset($_ENV['Path']) && substr($_ENV['Path'], 0,1) == '/') || (isset($_ENV['path']) && substr($_ENV['path'], 0,1) == '/'))))
		$GLOBALS['_os']='linux';

	// Set global newline for display
	$GLOBALS['nl']="\r\n";

	// Set global file separator according to OS
	$GLOBALS['_oss']='\\';
	if($GLOBALS['_os'] != 'windows')
		$GLOBALS['_oss']='/';

	// Set global newline for file actions according to OS
	$GLOBALS['nl_b']=$GLOBALS['nl'];
	if($GLOBALS['_os'] != 'windows')
		$GLOBALS['nl_b']="\n";

	// Server variables
	$GLOBALS['__server_array']=array('addr'=>'', 'port'=>80, 'name'=>'', 'request_uri'=>'', 'script_name'=>'', 'prg'=>'', 'url'=>'http', 'urlr'=>'http', 'urls'=>'http');
	if(fxIsArray($_SERVER))
	{
		if(isset($_SERVER['SERVER_NAME']))
			$GLOBALS['__server_array']['name']=$_SERVER['SERVER_NAME'];
		if(isset($_SERVER['SERVER_ADDR']))
			$GLOBALS['__server_array']['addr']=$_SERVER['SERVER_ADDR'];
		if(isset($_SERVER['SERVER_PORT']))
			$GLOBALS['__server_array']['port']=(int)$_SERVER['SERVER_PORT'];
		if(isset($_SERVER['REQUEST_URI']))
		{
			$GLOBALS['__server_array']['request_uri']=$_SERVER['REQUEST_URI'];
			$GLOBALS['__server_array']['prg']=basename($_SERVER['REQUEST_URI']);
			$qm=strpos($GLOBALS['__server_array']['prg'],'?');
			if($qm !== false)
				$GLOBALS['__server_array']['prg']=trim(substr($GLOBALS['__server_array']['prg'],0,$qm));
		}
		if(isset($_SERVER['SCRIPT_NAME']))
			$GLOBALS['__server_array']['script_name']=$_SERVER['SCRIPT_NAME'];

		if($GLOBALS['__server_array']['port'] == 443)
			$GLOBALS['__server_array']['url'] .= 's';
		$GLOBALS['__server_array']['url'] .= '://';
		if(strlen($GLOBALS['__server_array']['name']))
			$GLOBALS['__server_array']['url'] .= $GLOBALS['__server_array']['name'];
		else if(strlen($GLOBALS['__server_array']['addr']))
			$GLOBALS['__server_array']['url'] .= $GLOBALS['__server_array']['addr'];
		else
			$GLOBALS['__server_array']['url'] .= 'localhost';
		if(($GLOBALS['__server_array']['port'] != 80) && ($GLOBALS['__server_array']['port'] != 443))
			$GLOBALS['__server_array']['url'] .= ':'.$GLOBALS['__server_array']['port'];

		$GLOBALS['__server_array']['urlr']=$GLOBALS['__server_array']['url'].$GLOBALS['__server_array']['request_uri'];
		$GLOBALS['__server_array']['urls']=$GLOBALS['__server_array']['url'].$GLOBALS['__server_array']['script_name'];

		if(strlen($GLOBALS['__server_array']['script_name']))
			$burl=$GLOBALS['__server_array']['urls'];
		else
			$burl=$GLOBALS['__server_array']['urlr'];
		$lsp=strrpos($burl,'/');
		if($lsp)
			$burl=substr($burl,0,$lsp+1);
		$GLOBALS['__server_array']['url']=$burl;
	}
fxDebug($GLOBALS['__server_array'], '$GLOBALS[\'__server_array\']', 128);
}

/**
 * Set current session and return as parameters
 *
 * @param boolean $all - Optional parameter (default = true):  Return all paramters or just essentials?
 *
 * @return Current session parameter
 */
function fxSetSession($all=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$chklocsesarr=array('locstoid','sesstoid','locseskey','locsescnt','emode','lts');
	foreach($chklocsesarr as $chklocses)
	{
		if(!isset($GLOBALS[$chklocses]))
		{
			$GLOBALS[$chklocses]='';
			if(!strlen(${$chklocses}) && isset($_POST) && is_array($_POST) && sizeof($_POST) && isset($_POST[$chklocses]))
				$GLOBALS[$chklocses]=trim($_POST[$chklocses]);
			if(!strlen(${$chklocses}) && isset($_GET) && is_array($_GET) && sizeof($_GET) && isset($_GET[$chklocses]))
				$GLOBALS[$chklocses]=trim($_GET[$chklocses]);

			if($chklocses == 'locsescnt')
				$GLOBALS[$chklocses]=(int)$GLOBALS[$chklocses];
			if(($chklocses == 'lts') && !strlen($GLOBALS[$chklocses]))
				$GLOBALS[$chklocses]=date('YmdHis');
		}
	}

	// Check if key matches local and storage session
	$GLOBALS['locseschecked']=0;
	if((strlen($GLOBALS['locstoid']) == 15) && (substr($GLOBALS['locstoid'],0,1) == 'L') && (strlen($GLOBALS['sesstoid']) == 18) && (substr($GLOBALS['sesstoid'],0,1) == 'S'))
	{
		$cip='aes-256-cbc';
		$cil=openssl_cipher_iv_length($cip);
		$lsp=$GLOBALS['usrpath'].$GLOBALS['locstoid'].'/'.$GLOBALS['sesstoid'].'/';
		$lsf=$lsp.'k.pif';
//echo(__LINE__.': START -- $lsf=['.$lsf.'] -- $GLOBALS[\'locseskey\']=['.$GLOBALS['locseskey'].'] -- $lsk=['.$lsk.']<hr />');
		if(file_exists($lsf))
		{
			$lsa=fxLoad($lsf, 1);
			if(fxIsArray($lsa))
			{
				$lsk=trim($lsa[0]);
				if(!strlen($lsk) || (strlen($GLOBALS['locseskey']) && ($lsk !== $GLOBALS['locseskey'])))
					$GLOBALS['locseskey']='new';
				else if(sizeof($lsa) > 1)
				{
					$sdc=base64_decode(trim($lsa[1]));
					$rpb=substr($sdc,0,$cil);
					$e64=substr($sdc,$cil,64);
					$e32=substr($sdc,$cil+64);
					$sec=openssl_decrypt($e32, $cip, $lsk, OPENSSL_RAW_DATA, $rpb);
					$h64=hash_hmac('sha3-512', $e32, $lsk, true);
					if(!hash_equals($e64, $h64) || ($sec !== $GLOBALS['locstoid'].$GLOBALS['sesstoid']))
						$GLOBALS['locseskey']='set';
					else
						$GLOBALS['locseschecked']=1;
				}
				else
					$GLOBALS['locseskey']='set';
			}
//echo(__LINE__.': LOAD -- $lsk=['.$lsk.'] -- $GLOBALS[\'locseskey\']=['.$GLOBALS['locseskey'].'] -- $GLOBALS[\'locseschecked\']=['.$GLOBALS['locseschecked'].'] -- $sdc=['.$sdc.'] -- $sec=['.$sec.']<hr />'); die;
		}
		if(!strlen($GLOBALS['locseskey']) || ($GLOBALS['locseskey'] === 'set') || !$GLOBALS['locseschecked'])
		{
			$GLOBALS['locseskey']=hexPassword(32);
			$rpb=openssl_random_pseudo_bytes($cil);
			$r32=openssl_encrypt($GLOBALS['locstoid'].$GLOBALS['sesstoid'], $cip, $GLOBALS['locseskey'], OPENSSL_RAW_DATA, $rpb);
			$r64=hash_hmac('sha3-512',$r32,$GLOBALS['locseskey'],true);
			$scd=$GLOBALS['locseskey'].$GLOBALS['nl_b'].base64_encode($rpb.$r64.$r32);
			fxCreatePath($lsp);
			fxSave($lsf, $scd, true, false);
//echo(__LINE__.': SAVE -- $GLOBALS[\'locseskey\']=['.$GLOBALS['locseskey'].'] -- $GLOBALS[\'locseschecked\']=['.$GLOBALS['locseschecked'].'] -- $scd=['.$scd.']<hr />');
		}
//echo(__LINE__.': FINAL -- $GLOBALS[\'locseskey\']=['.$GLOBALS['locseskey'].'] -- $GLOBALS[\'locseschecked\']=['.$GLOBALS['locseschecked'].']<hr />');
	}

	$sparam='';
	foreach($chklocsesarr as $chklocses)
	{
		if($all || ($chklocses != 'locsescnt'))
			$sparam .= '&'.$chklocses.'='.$GLOBALS[$chklocses];
	}
//fxDebug($sparam,'$sparam', 0);

	return $sparam;
}

/**
 * Set and determine the current fx-project version
 */
function fxSetVersion()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['v_type']='Community';
	$GLOBALS['v_tcol']='#916c53';
	if(function_exists('fxsppSetVersion'))
		fxsppSetVersion();

	$GLOBALS['v_fxp']=FXP_VERSION;
	while(substr($GLOBALS['v_fxp'],-2) == '.0')
		$GLOBALS['v_fxp']=substr($v_fxp,0,-2);
}

/**
 * Return the current date with or without the current time in our format
 *
 * @param boolean $with_time - Optional parameter (default = true):  Return also time?
 *
 * @return Current date with or without the current time
 */
function fxNow($with_time=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($with_time)
		return fxDate('YmdHis');
	return fxDate();
}

/**
 * Return the converted unixtime
 *
 * @param string  $format   - Optional parameter (default = 'Ymd'):  Format that the unixtime should be converted into
 * @param integer $unixtime - Optional parameter (default = 0):      Excplit unixtime that should be converted or 0 for the current unixtime
 *
 * @return Converted unixtime into specified format
 */
function fxDate($format='Ymd', $unixtime=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($format))
		$format='Ymd';
	if(!$unixtime)
		$unixtime=time();

	return date($format, $unixtime);
}

/**
 * Set all dynamic paths into global variables, e.g. the SYS-subirectory will be stored in the global variable $GLOBALS['syspath'] etc.
 * Mandatory first and second level subdirectories are: AJX, CLS, CSS, DAT, EXT, GFX, INT, JSC, KEY, LNG, LOG, PRG, SPP, SYS + MIC, TMP, USR
 * -> All mandatory first level subdirectories are inserted into PHP's "include_path"
 * -> All scanned dynamic paths are saved in the file "_fxpathdir.fpi" in the main directory
 * -> All scanned open source externals are saved in the file "_fxextfiles.fpi" in the main directory
 * -> All scanned php files are saved in the file "_fxphpfiles.fpi" in the main directory
 * -> All scanned program paths are saved in the file "_fxprgdir.fpi" in the PRG-subdirectory
 * -> All scanned special program paths are saved in the file "_fxsppdir.fpi" in the SPP-subdirectory
 *
 * @param boolean $exit_on_error - Optional parameter (default = true):  Leave function if there is an error, e.g. a dynamic path could not be found
 * @param boolean $rescan_pdirs - Optional parameter (default = false):  Create the special files "_fxprgdir.fpi" and "_fxsppdir.fpi" for all subdirectories in the main directory (will be done automatically once a day)
 */
function fxSetDynamicPaths($exit_on_error=true, $rescan_pdirs=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$inc_files=true;	// Include existing files? Set to FALSE only for debugging
	$error_list='';

	// Get current PHP include path
	$oip=ini_get('include_path');
	$ois=';';
	if($GLOBALS['_os'] != 'windows')
		$ois=':';
//echo('$GLOBALS[\'_os\']: <b>'.$GLOBALS['_os'].'</b><br />include_path: <b>'.$oip.'</b><hr />');
	// Get dynamic main path, i.e. path where this program is
	$bfilename='basics.inc';
	if(!isset($GLOBALS['mainpath']) || !file_exists($GLOBALS['mainpath'].$bfilename))
	{
		$path='';
		for($spc=0; $spc<10; $pc++)
		{
			if(file_exists($path.$bfilename))
			{
				if(substr($path,-1) != '/')
					$path .= '/';
				break;
			}
			$path .= '../';
		}
		if($path == '/')
			$path='./';

		$GLOBALS['mainpath']=$path;
	}
fxDebug($GLOBALS['mainpath'], '$GLOBALS[\'mainpath\']',2);
	if(!file_exists($GLOBALS['mainpath'].$bfilename))
		$error_list .= '<li>[MAIN] &hellip; '.$bfilename.'</li>';
	else
		$oip .= $ois.substr($GLOBALS['mainpath'],0,-1);

/*
	$GLOBALS['subpath']='';
	if(fxIsArray($GLOBALS['__server_array']))
	{
		$requri=$GLOBALS['__server_array']['request_uri'];
		// Cut parameters?
		$qp=strpos($requri,'?');
		if($qp)
			$requri=substr($requri,0,$qp);
		$GLOBALS['subpath']=str_repeat('../', substr_count($requri,'/')-substr_count($GLOBALS['__server_array']['script_name'],'/'));
	}
	if(!strlen($GLOBALS['subpath']))
		$GLOBALS['subpath']=$GLOBALS['mainpath'];
fxDebug($GLOBALS['subpath'], '$GLOBALS[\'subpath\']',2);
*/
	$GLOBALS['subpath']=$GLOBALS['mainpath'];

	$mpl=strlen($GLOBALS['mainpath']);

	// Get all other dynamic subpaths and externals according to the main path
	if(!strlen($error_list))
	{
		$GLOBALS['_subpath_array']=array();

		$pathfile=$GLOBALS['mainpath'].'_fxpathdir.fpi'; $pathdate='00000000';
		if($inc_files && file_exists($pathfile))
		{
			$pathdate=date('Ymd', filemtime($pathfile));
			require($pathfile);
		}

		$GLOBALS['_ext_array']=array();

		$extfile=$GLOBALS['mainpath'].'_fxextfiles.fpi'; $extdate='00000000';
		if($inc_files && file_exists($extfile))
		{
			$extdate=date('Ymd', filemtime($extfile));
			require($extfile);
		}

		// Paths not found or file outdated -> Create new one
		if(!sizeof($GLOBALS['_subpath_array']) || ((int)$pathdate < (int)$GLOBALS['date']))
		{
			$GLOBALS['_subpath_array']=array(); $pathdate='99999999';
			$GLOBALS['_ext_array']=array(); $extdate='99999999';
			$spa=fxScanDir($GLOBALS['mainpath'], true, false, '', true);
			if($spa['dir_entries'] > 0)
			{
				foreach($spa['dirs'] as $dname => $dpath)
				{
					// We check the first char of the directory name
					$d1=substr($dname,0,1);
					// We don't want PRG, SPP or USR subdirectories (yet)
					$ip=stripos($dpath,'PRG/');
					if($ip === false)
						$ip=stripos($dpath,'SPP/');
					if($ip === false)
						$ip=stripos($dpath,'USR/');
					// We only want 1st level directories, except for DAT, DOC
					if($ip === false)
					{
						$pl=substr_count($dpath,'/');
						if($pl > 2)
							$ip=true;
						else
						{
							$dp=stripos($dpath,'DAT/');
							if($dp === false)
								$dp=stripos($dpath,'DOC/');
							if(($dp === false) && ($pl > 1))
								$ip=true;
						}
					}
					if(($ip === false) && ($d1 != '_') && ($d1 != '0') && ((int)$d1 == 0))
						$GLOBALS['_subpath_array'][strtoupper(strtr($dname, array('['=>'_', ']'=>'', ' '=>'', '-'=>'')))]=$dpath.'/';
				}

				// Check for open source externals in the EXT subdirectory
				if(isset($GLOBALS['_subpath_array']['EXT']))
				{
					$eca=array('phpchartdir.php'=>'CHARTDIRECTOR', 'fpdf.php'=>'FPDF', 'fpdi.php'=>'FPDI', 'phpmailer.php'=>'PHPMAILER', 'spreadsheet.php'=>'PHPSPREADSHEET', 'tcpdf.php'=>'TCPDF');
					$efa=fxScanDir($GLOBALS['_subpath_array']['EXT'], false, true, '/^.+\.php$/i', true);
					if($efa['file_entries'] > 0)
					{
						foreach($efa['files'] as $filename => $filepath)
						{
							$pl=substr_count($filepath,'/');
							$lfilename=strtolower($filename);
							if(isset($eca[$lfilename]) && ($pl < 6))	// subdir level restriction < 6, or else TCPDF would be found in PHPSPREADSHEET as a subdir
							{
								$GLOBALS['_ext_array'][$eca[$lfilename]]=$filepath;
								$GLOBALS['_subpath_array'][$eca[$lfilename]]=substr($filepath,0,-strlen($lfilename));
							}
						}
					}
				}
			}
		}
		else
		{
			if(fxIsArray($GLOBALS['_subpath_array']))
			{
				foreach($GLOBALS['_subpath_array'] as $spdname => $spdpath)
					$GLOBALS['_subpath_array'][$spdname]=$GLOBALS['mainpath'].$spdpath;
			}
			if(fxIsArray($GLOBALS['_ext_array']))
			{
				foreach($GLOBALS['_ext_array'] as $edname => $edpath)
					$GLOBALS['_ext_array'][$edname]=$GLOBALS['mainpath'].$edpath;
			}
		}
//fxDebug($GLOBALS['_subpath_array'], '$GLOBALS[\'_subpath_array\']', 2);
//fxDebug($GLOBALS['_ext_array'], '$GLOBALS[\'_ext_array\']', 2);

		// Check for mandatory paths
		$mpa=array('AJX', 'CLS', 'CSS', 'DAT', 'DOC', 'EXT', 'GFX', 'INT', 'JSC', 'KEY', 'LNG', 'LOG', 'MIC', 'PRG', 'SPP', 'SYS',  'TMP', 'USR');
		foreach($mpa as $mpdname)
		{
			if(!sizeof($GLOBALS['_subpath_array']) || !isset($GLOBALS['_subpath_array'][$mpdname]))
				$error_list .= '<li>'.$GLOBALS['mainpath'].$mpdname.'/</li>';
			else
				$oip .= $ois.substr($GLOBALS['_subpath_array'][$mpdname],0,-1);
		}

		// Save paths
		if(!strlen($error_list) && ($pathdate == '99999999') && fxIsArray($GLOBALS['_subpath_array']))
		{
			ksort($GLOBALS['_subpath_array'], SORT_STRING);
			fxDelete($pathfile);

			$inc  = '<'.'?'.'php'.$GLOBALS['nl_b'];
			$inc .= '// Please don\'t change this file, because it will be created daily in fx-project in the integrated function: '.$bfilename.' -> fxSetDynamicPaths(true);'.$GLOBALS['nl_b'];
			$inc .= '$GLOBALS[\'_subpath_array\']=array('.$GLOBALS['nl_b'];
			$dc=0;
			foreach($GLOBALS['_subpath_array'] as $spdname => $spdpath)
			{
				if($dc)
					$inc .= ','.$GLOBALS['nl_b'];
				$inc .= '	\''.$spdname.'\' => \''.str_replace('\'', '\\\'', substr($spdpath, $mpl)).'\'';
				$dc++;
			}
			$inc .= $GLOBALS['nl_b'].');'.$GLOBALS['nl_b'];
			$inc .= '?'.'>';

			fxSave($pathfile, $inc, true);
		}

		// Save externals
		if(!strlen($error_list) && ($extdate == '99999999') && fxIsArray($GLOBALS['_ext_array']))
		{
			ksort($GLOBALS['_ext_array'], SORT_STRING);
			fxDelete($extfile);

			$inc  = '<'.'?'.'php'.$GLOBALS['nl_b'];
			$inc .= '// Please don\'t change this file, because it will be created daily in fx-project in the integrated function: '.$bfilename.' -> fxSetDynamicPaths(true);'.$GLOBALS['nl_b'];
			$inc .= '$GLOBALS[\'_ext_array\']=array('.$GLOBALS['nl_b'];
			$dc=0;
			foreach($GLOBALS['_ext_array'] as $edname => $edpath)
			{
				if($dc)
					$inc .= ','.$GLOBALS['nl_b'];
				$inc .= '	\''.$edname.'\' => \''.str_replace('\'', '\\\'', substr($edpath, $mpl)).'\'';
				$dc++;
			}
			$inc .= $GLOBALS['nl_b'].');'.$GLOBALS['nl_b'];
			$inc .= '?'.'>';

			fxSave($extfile, $inc, true);
		}

		if(sizeof($GLOBALS['_subpath_array']))
		{
			$slmp=strlen($GLOBALS['mainpath']);
			foreach($GLOBALS['_subpath_array'] as $spdname => $spdpath)
				$GLOBALS[strtolower($spdname).'path']=$GLOBALS['subpath'].substr($spdpath,$mpl);
		}
	}

	// Get all php files
	$GLOBALS['_phpfiles']=$GLOBALS['mainpath'].'_fxphpfiles.fpi';
	if(!strlen($error_list))
	{
		$ppa=array();

		$phpdate='00000000';
		if($inc_files && file_exists($GLOBALS['_phpfiles']))
		{
			$phpdate=date('Ymd', filemtime($GLOBALS['_phpfiles']));
			require($GLOBALS['_phpfiles']);
		}

		// File not found or file outdated -> Create new one
		if(!sizeof($ppa) || ((int)$phpdate < (int)$GLOBALS['date']))
		{
			$ppa=array();
			$phpdate='99999999';

			// Scan special directories for .php and .inc files
			$mpl=strlen($GLOBALS['mainpath']);
			$dfa=array('entries'=>0, 'all'=>array());
			$sda=array('main', 'ajx', 'app', 'cls', 'int', 'prg', 'spp', 'sys');
			foreach($sda as $sd)
			{
				if($sd == 'main')
					$recursive=false;
				else
					$recursive=true;
				$sfa=fxScanDir($GLOBALS[$sd.'path'], false, true, '/^.+\.(php|inc)$/i', $recursive);
//fxDebug($sfa,'$sfa', 0);
				if($sfa['entries'] > 0)
				{
					$dfa['entries'] += $sfa['entries'];
					foreach($sfa['all'] as $filepath => $fa)
						$dfa['all'][$filepath]=$fa;
				}
			}
//fxDebug($dfa,'$dfa', 0);
			if($dfa['entries'] > 0)
			{
				foreach($dfa['all'] as $filepath => $fa)
				{
					// File path
					$fpath=substr($fa['path'],$mpl);
					if(!$fpath)
						$fpath='';
					$ppa[substr($filepath,$mpl)]=array('filepath'=>$fpath, 'filename'=>$fa['name']);
				}
			}

			fxDelete($GLOBALS['_phpfiles']);

			// Save file array
			$inc  = '<'.'?'.'php'.$GLOBALS['nl_b'];
			$inc .= '// Please don\'t change this file, because it was created in fx-project with the program file: '.basename(__FILE__).$GLOBALS['nl_b'];
			$inc .= fxWriteVar('=$ppa=',$ppa);
			$inc .= ';'.$GLOBALS['nl_b'].'?'.'>';

			fxSave($GLOBALS['_phpfiles'], $inc);
		}
//fxDebug($ppa,'$ppa', 0);
	}

	// Default CSS file
	$GLOBALS['cssfile']='00default.css';
	if(!strlen($error_list) && is_dir($GLOBALS['_subpath_array']['CSS']))
	{
		if(!file_exists($GLOBALS['_subpath_array']['CSS'].$GLOBALS['cssfile']))
			$error_list .= '<li>'.$GLOBALS['cssfile'].'</li>';
		else
			$GLOBALS['cssfile']=$GLOBALS['csspath'].$GLOBALS['cssfile'];
	}
fxDebug($GLOBALS['cssfile'],'$GLOBALS[\'cssfile\']', 2);

	// Get all program and special program function paths
	$pda=array('prg', 'spp');
	foreach($pda as $pdn)
	{
		$GLOBALS[$pdn.'file']=$GLOBALS['mainpath'].'_fx'.$pdn.'dir.fpi';
		$GLOBALS[$pdn.'dir']=array();
		if(!strlen($error_list) && is_dir($GLOBALS[$pdn.'path']))
		{
			// Program paths already scanned and no rescan wanted or necessary?
			$pdate='00000000';
			if(!$rescan_pdirs && file_exists($GLOBALS[$pdn.'file']))
			{
				$pdate=date('Ymd', filemtime($GLOBALS[$pdn.'file']));
				if((int)$pdate < (int)$GLOBALS['date'])
					$rescan_pdirs=true;
				else if($inc_files)
				{
					require($GLOBALS[$pdn.'file']);
				}
			}

			// Scan for program subdirectories
			if(!sizeof($GLOBALS[$pdn.'dir']) || $rescan_pdirs)
			{
				$plen=strlen($GLOBALS[$pdn.'path']);
				$dfa=fxScanDir($GLOBALS[$pdn.'path'], true, false);
//fxDebug($dfa,'$dfa', 3);
				// ...Subdirectories found?
				if($dfa['dir_entries'] > 0)
				{
					// ...Yes, check each directory if it starts with 3 digits
					foreach($dfa['dirs'] as $dname => $dpath)
					{
						// ...Yes again, add it to program function path array
						if((strlen($dname) > 2) && is_numeric(substr($dname,0,1)) && is_numeric(substr($dname,1,1)) && is_numeric(substr($dname,2,1)))
							$GLOBALS[$pdn.'dir'][(int)substr($dname,0,3)]=$dname;
					}
				}

				// Save program paths
				fxDelete($GLOBALS[$pdn.'file']);
				if(sizeof($GLOBALS[$pdn.'dir']))
				{
					$inc  = '<'.'?'.'php'.$GLOBALS['nl_b'];
					$inc .= '// Please don\'t change this file, because it will be created daily in fx-project in the integrated function: '.$bfilename.' -> fxSetDynamicPaths(true, true);'.$GLOBALS['nl_b'];
					$inc .= '$GLOBALS[\''.$pdn.'dir\']=array('.$GLOBALS['nl_b'];
					$pc=0;
					foreach($GLOBALS[$pdn.'dir'] as $pnr => $ppath)
					{
						if($pc)
							$inc .= ','.$GLOBALS['nl_b'];
						$inc .= '	'.$pnr.' => \''.str_replace('\'', '\\\'', $ppath).'\'';
						$pc++;
					}
					$inc .= $GLOBALS['nl_b'].');'.$GLOBALS['nl_b'];
					$inc .= '?'.'>';

					fxSave($GLOBALS[$pdn.'file'], $inc, true);
				}
			}

			if(sizeof($GLOBALS[$pdn.'dir']))
			{
				foreach($GLOBALS[$pdn.'dir'] as $pnr => $ppath)
					$GLOBALS[$pdn.'dir'][$pnr]=$GLOBALS[$pdn.'path'].$ppath;
			}
		}
//fxDebug($GLOBALS[$pdn.'file'], '$GLOBALS[\''.$pdn.'file\']', 2);
//fxDebug($GLOBALS[$pdn.'dir'], '$GLOBALS[\''.$pdn.'dir\']', 2);
	}

	// Set new PHP include path
	ini_set('include_path',$oip);
//$nip=ini_get('include_path'); echo('include_path: <b>'.$nip.'</b><hr />');

	// If one or more paths were not found display error and exit program
	if($exit_on_error && strlen($error_list))
		die('<h1 style="padding:8px;color:red;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: The following necessary paths or files were not found:<ul>'.$error_list.'</ul></h1>');
}

/**
 * Get fx-project favicon
 *
 * @return fx-project favicon
 */
function fxGetFavicon()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(function_exists('fxsppGetFavicon'))
		return fxsppGetFavicon();

	return 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAACXBIWXMAAA7DAAAOwwHHb6hkAAADp0lEQVR4nFVTW2wUZRg9M/23e+tO2213e2dLL7RpCmlpAFvCxbZctQlVYww2EV+MCS8aTNBANIUHaozKi/HBYNJIxCBCStQgISldoQUspS0G6nbZTafb7Za9787MzmVnxtGkJn5PX/LlnJzv5Byi6zrWhwsHemauXzzBLsztpixFpTRTVmB2MMZFV7W8mnBW1Iy37+z/3O1pfrCOIetLOrJ8MvR4clhMrJiD4RQiKEC+BDBZNVS6igsqrIqLD/pfX5ydGti+/5VTnf1HvvyPQEhGP1wLBc8lcxSmc01w9PXjYHcnNE3Hua+vIcTqqHLXoqmsFo78I+vE1dEvNEN6177B8yQvcj0rgYUzsqRgIeZGZVsdDvZtwbeXxxGLxWAzWaDThQYZDdrRCHP5BijKFXjHvh+pbmy7Q/jlhY+UxLJpPuyAQqqxrZWBLxDHw3kWopRFU0MLCLFDkGj4V3h0NLth3rAHXPqKeerm2Eky5/1tV45isJJzGc8kYLU4ISsS3hrcBmKi8cPP99FQtwn1bgem/UkEomm4bU6YSjeCDfh6SS6bZnyiFVEta8jNo8zJQM6LKC62Yy74DCaL3TBKReh5BOWUgLpLF8B37YZYUwkixEqILElUJJlDXI0iQ6sIryXw041ZUGkOtgde1DS2gHe7IRkkDb+MYtef93CpqR2Cowh2WacNBSkocjHyugxQKgRBRE6QYEchOoNBlE3ewvjAEBbLXagL+HGv52WEKqvhyCwhk1wFUbi0zhCFEq0uWAr+yYwJzVWtyMkqvuk7gtanMzgwfgMdShbC5h34a+BNOMUc5Mg0Ms9XNSKrapbRIoxesRd2sw1LARW0QeJjn4AXMphq7QCTjOIwm8Eqz8HDCcg4nfAvzQCakCYlNY13U4//OFRSsQKLswdcVgYbDSGSXINGTKj2zWIT+ww33zmLKqsd5QUEmfgCYuFFdPcdniBbXzo64nt4d5/mv0oKbTWIaQ6E4mFIedlwmcOBR5Pwdu9HgtaguowkajH4x0ZhdpTIr757YoR4Nm/3vjB47Oyti18NizPnkS3fCz5ngkQVot3/FGFPG+ar6+HmEkixtxH2/wqBT2HovdMft2zdcf/fLuw5evyMqoEa//HCaTp9mdTba1FaWAW9oQyLRcVoopZRxMfBhVioOiUfe//UJ4feePvT/7Wxd+j4sKe9a8J77bsPQotPdlq4KKMbUbTxFCjDLksRk67t6f39xYHXPmts23JnHfc37gqoq1wjNe8AAAAASUVORK5CYII=';
}

/**
 * Get fx-project logo
 *
 * @param string $big - Optional parameter (default = 'big'):  Get big logo for loading screen (= 'big') or medium logo for email (= 'medium') or small logo for toolbar (= 'small')
 *
 * @return fx-project logo
 */
function fxGetLogo($size='big')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(function_exists('fxsppGetLogo'))
		return fxsppGetLogo($size);

	$size=strtolower(substr(trim($size),0,1));
	if($size == 'b')
		$logo='data:image/png;base64,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';
	else if($size == 'm')
		$logo='data:image/png;base64,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';
	else
		$logo='data:image/png;base64,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';

	return $logo;
}

/**
 * Load a text file, convert the encoding if necessary and create an optional array from it
 *
 * @param various $filename     - Mandatory parameter:               Path and name of the text file to be loaded
 * @param integer $line_by_line - Optional parameter (default = 0):  Create an array from the text file with line number as key and line text as value - if > 1 also return empty lines, else only non empty lines
 *
 * @return Loaded text file or text array
 */
function fxLoad($filename, $line_by_line=0)
{
    // Display function call?
    if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

    if(!$filename || !file_exists($filename))
        return false;

	$content=fxLoadBin($filename, '');
	if(($content !== false) && strlen($content) && function_exists('mb_detect_encoding'))
	{
		$oe=mb_detect_encoding($content, 'UTF-8, ISO-8859-1, ISO-8859-15', true);
//echo('fxLoad: $oe=<b>'.$oe.'</b> &rarr; <b>'.FXP_CHARSET.'</b><hr />');
		if($oe != FXP_CHARSET)
			$content=mb_convert_encoding($content, FXP_CHARSET, $oe);
	}

	if(!$line_by_line || ($content === false) || !strlen($content))
	    return $content;

	// Return file line by line
	$ca=array();
	$ar=explode("\n", strtr($content, array("\r\n"=>"\n", "\r"=>"\n")));
	foreach($ar as $ln => $lc)
	{
		$lt=trim($lc);
		if(($line_by_line > 1) || strlen($lt))
		{
			if(!strlen($lc))
				$lc='';
			$ca[]=$lc;
		}
	}

	return $ca;
}

/**
 * Load a binary file
 *
 * @param various $filename - Mandatory parameter:                Path and name of the text file to be loaded
 * @param string  $mode     - Optional parameter (default = 'b'): Additional string for php's fopen, here 'b' for binary
 *
 * @return Loaded binary file
 */
function fxLoadBin($filename, $mode='b')
{
    // Display function call?
    if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

    if(!$filename || !file_exists($filename))
        return false;

	$size=filesize($filename);

    $content=false;
	if($size)
	{
		$fp=fopen($filename, 'r'.$mode);
		if($fp)
		{
			$content=fread($fp, $size);
			fclose($fp);
		}
	}

    return $content;
}

/**
 * Save a text file with converted encoding if necessary
 *
 * @param various $filename  - Mandatory parameter:                  Path and name of the text file to be created or appended
 * @param various $content   - Mandatory parameter:                  Text to be saved
 * @param boolean $clearfile - Optional parameter (default = true):  Create a new text file (true) or append text to an existing file (false)
 * @param boolean $addnl     - Optional parameter (default = true):  Add a newline to an existing file if a text should be appended (true) or not (false)
 *
 * @return Status, i.e. true on error (text could not be saved) or false on success
 */
function fxSave($filename, $content, $clearfile=true, $addnl=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$err=false;
	$fname=trim($filename);
	if($clearfile)
		$filemode='w+';
	else
		$filemode='a+';

	if(is_array($content))
	{
		$a=$content;
		$content='';
		if(sizeof($a))
		{
			foreach($a as $line)
				$content .= $line.$GLOBALS['nl_b'];
		}
	}

	if(!$clearfile && $addnl && file_exists($fname))
		$content=$GLOBALS['nl_b'].$content;

	$fp=@fopen($fname, $filemode);
	if($fp)
	{
		flock($fp, LOCK_EX);
		if(function_exists('mb_detect_encoding'))
		{
			$oe=mb_detect_encoding($content, 'UTF-8, ISO-8859-1, ISO-8859-15', true);
//echo('fxSave: $filename='.$filename.' -- $oe=<b>'.$oe.'</b> &rarr; <b>'.FXP_CHARSET.'</b><hr />');
			if($oe != FXP_CHARSET)
			{
				$content=mb_convert_encoding($content, FXP_CHARSET, $oe);
//echo('<pre>'.fxHtmlEncode($content).'</pre>');
			}
		}

		$bw=fwrite($fp, $content);
		if($bw === false)
			$err=true;
		flock($fp, LOCK_UN);
		fclose($fp);
	}
	else
		$err=true;

	return $err;
}

/**
 * Save a binary file
 *
 * @param various $filename  - Mandatory parameter:  Path and name of the binary file to be created
 * @param various $content   - Mandatory parameter:  Data to be saved
 *
 * @return Status, i.e. true on error (data could not be saved) or false on success
 */
function fxSaveBin($filename, $content)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$err=false;
	$fname=trim($filename);
	$filemode='wb+';

	$fp=@fopen($fname, $filemode);
	if($fp)
	{
		flock($fp, LOCK_EX);
		$bw=fwrite($fp, $content);
		if($bw === false)
			$err=true;
		flock($fp, LOCK_UN);
		fclose($fp);
	}
	else
		$err=true;

	return $err;
}

/**
 * Delete a file, files in a directory or a complete dirctory
 *
 * @param string  $filepath     - Mandatory parameter:                     Path and name of the text file or the directory to be deleted
 * @param string  $filter_files - Optional parameter (default = ''):       Delete only specific files, then filter has to be provided as a regular expression
 * @param integer $filter_time  - Optional parameter (default = 0):        Delete only specific files older than x seconds
 * @param boolean $del_dir      - Optional parameter (default = false):    Delete file (false) or files and/or directory (true)
 * @param array   $ignore       - Optional parameter (default = array()):  Array of filenames to be ignored, i.e. not to be deleted
 *
 * @return Status, i.e. true on success (file(s) or directory was deleted) or false on error
 */
function fxDelete($filepath, $filter_files='', $filter_time=0, $del_dir=false, $ignore=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$filepath=trim($filepath);

	// Delete specified file
	if(!$del_dir)
	{
		// ...delete it
		if(file_exists($filepath) && is_file($filepath))
		{
			@chmod($filepath, 0777);
			@unlink($filepath);
		}
		// ...return error, if file still exists
		if(file_exists($filepath))
			return false;
	}
	// Delete file(s) in a directory or complete directory
	else if(is_dir($filepath))
	{
		// ...scan complete directory
		$dfa=fxScanDir($filepath, true, true, $filter_files, true);
//fxDebug($dfa,'$dfa: fxDelete(\''.$filepath.'\')', 0);

		// ...delete file(s)
		if($dfa['file_entries'] > 0)
		{
			$dt=time()-$filter_time;;
			foreach($dfa['files'] as $name => $path)
			{
				if((!$filter_time || (filectime($path) < $dt)) && (!fxIsArray($ignore) || !isset($ignore[$path])))
				{
					@chmod($path, 0777);
					@unlink($path);
					// ...return error, if file still exists
					if(file_exists($path))
						return false;
				}
			}
		}

		// ...delete directories
		if(!strlen($filter_files) && !$filter_time && !fxIsArray($ignore))
		{
			// ...delete subdirectories
			$da=$dfa['dirs'];
			while(fxIsArray($da))
			{
				// ...get and delete highest level subdirectory
				$lvl=0;
				$name='';
				$path='';
				foreach($da as $n => $p)
				{
					$l=substr_count($p,'/');
					if(!$lvl || ($l > $lvl))
					{
						$lvl=$l;
						$name=$n;
						$path=$p;
					}
				}
				@chmod($path, 0777);
				@rmdir($path);
				// ...return error, if subdirectory still exists
				if(is_dir($path))
					return false;

				unset($da[$name]);
			}

			// ...delete directory
			@chmod($filepath, 0777);
			@rmdir($filepath);
			// ...return error, if file directory exists
			if(is_dir($filepath))
				return false;
		}
	}

	// Success
	return true;
}

/**
 * Delete older files in the TMP subdirectory
 *
 * @param integer $filter_time - Optional parameter (default = 3600):  Delete only specific files older than x seconds
 *
 * @return Status, i.e. true on success (file(s) were deleted) or false on error
 */
function fxDeleteTMP($filter_time=3600)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return fxDelete($GLOBALS['tmppath'], '', $filter_time, true, array('index.php'=>true));
}

/**
 * Delete older files in the LOG subdirectory
 *
 * @return Status, i.e. true on success (file(s) were deleted) or false on error
 */
function fxDeleteLOG()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Don't delete logs if setting value = 0 or not set
	$filter_time=(int)$GLOBALS['fxpglobals']['settings']['log_delete'];
	if($filter_time < 1)
		return true;

	return fxDelete($GLOBALS['logpath'], '', $filter_time, true, array('index.php'=>true));
}

/**
 * Create a directory path with all subpaths
 *
 * @param various $filepath - Mandatory parameter:  Directory path to be created
 *
 * @return Path that could not be created on error, or empty string on success
 */
function fxCreatePath($filepath)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$err='';
	$filepath=trim($filepath);
	if(strlen($filepath))
	{
		$filepath=str_replace('\\','/',$filepath);
		$patharray=explode('/', $filepath);
//fxDebug($patharray,'$patharray: $filepath='.$filepath, 0);
		$path='';
		foreach($patharray as $p)
		{
			if(strlen($path))
				$path .= '/';
			$path .= $p;
			if(($path != '.') && ($path != '..') && (strpos(substr($path,2),'.') === false))
			{
//echo('Check path: '.$path.'<hr />');
				if(!is_dir($path))
				{
//echo('&rarr; Create path: '.$path.'<hr />');
					@mkdir($path, 0777);
					if(!is_dir($path))
					{
						$err=$path;
						break;
					}
				}
			}
		}
	}

	return $err;
}

/**
 * Scan a directory and return all found subdirectories and files
 *
 * @param various $path         - Mandatory parameter:                   Directory path to be scanned
 * @param boolean $with_dirs    - Optional parameter (default = true):   Get directories (true) or not (false)
 * @param boolean $with_files   - Optional parameter (default = true):   Get files (true) or not (false)
 * @param string  $filter_files - Optional parameter (default = ''):     Get only specific files, then filter has to be provided as a regular expression
 * @param boolean $recursive    - Optional parameter (default = false):  Also scan all subdirectories (true) or not (false)
 *
 * @return Array that contains all found subdirectories and/or files
 */
function fxScanDir($path, $with_dirs=true, $with_files=true, $filter_files='', $recursive=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Prepare return array
	$dfa=array('entries'=>0, 'all'=>array(), 'dir_entries'=>0, 'dirs'=>array(), 'file_entries'=>0, 'files'=>array());

	if(strlen($path))
	{
		// Convert all backslashes to forward slashes and remove trailing forward slashes
		$path=str_replace('\\', '/', $path);
		while(substr($path,-1) == '/')
			$path=substr($path,0,-1);

		// Scan the directory if it exists
		if(is_dir($path))
		{
			$na=array($path=>true);
			while(sizeof($na))
			{
				$pa=$na;
				$na=array();
				foreach($pa as $p => $dummy)
				{
					$dh=@opendir($p);
					if($dh)
					{
						while(false !== ($f=@readdir($dh)))
						{
							// Skip parent dirs
							if(($f == '.') || ($f == '..'))
								continue;

							// Get path and file with type
							$pf=$p.'/'.$f;
							$df='file';
							if(is_dir($pf))
							{
								$df='dir';
								if($recursive)
									$na[$pf]=true;
							}

							// Check if file entry matches filter criteria
							if(($df == 'file') && strlen($filter_files) && !preg_match($filter_files, $f))
								continue;

							// Add entry
							if(($with_dirs && ($df == 'dir')) || ($with_files && ($df == 'file')))
							{
								$dfa['entries']++;
								$dfa['all'][$pf]=array('dir'=>(($df == 'dir')?true:false), 'path'=>$p, 'name'=>$f);

								$dfa[$df.'_entries']++;
								$ec=0;
								while(($ec < 100) && isset($dfa[$df.'s'][$f]))
								{
									$ec++;
									$fs=strrpos($f,'.');
									if($fs === false)
										$fs=strlen($f);
									if(substr($f,$fs-1,1) == ']')
									{
										$bs=strrpos($f,'[');
										if($bs === false)
											$f=substr($f,0,$fs).'[1]'.substr($f,$fs);
										else
											$f=substr($f,0,$bs+1).(1+(int)substr($f,$bs+1)).substr($f,$fs-1);
									}
									else
										$f=substr($f,0,$fs).'[1]'.substr($f,$fs);
								}
								$dfa[$df.'s'][$f]=$pf;
							}
						}
						@closedir($dh);
					}
				}
			}
		}
	}

	return $dfa;
}
/**
 * Return the requested filename and path ready to be included or The pathes to check
 *
 * @param string  $filename      - Mandatory parameter:                     Filename without suffix (if empty, the array of the pathes to check will be returned)
 * @param various $rpfa          - Optional parameter (default = array()):  Search also in requested program function path(s), e.g. $rpf=123; (as integer) or $rpf='123,456'; (as string) or $rpf=array(123,456); (as array)
 * @param string  $rp            - Optional parameter (default = ''):       Search also in requested path, e.g 'chartdirector' for the ChartDirector suppath
 * @param string  $rps           - Optional parameter (default = ''):       Search also in requested path + subpath
 *
 * @return Filename with path or Pathes to check
 */
function fxf_fn_reqFilename($filename, $rpfa=array(), $rp='', $rps='')
{
	$filename=trim($filename);
	if(!is_array($rpfa))
		$rpfa=explode(',', $rpfa);
	$rp=trim($rp);
	if(strlen($rp))
		$rp=strtolower($rp);
	$rps=trim($rps);
	if(strlen($rps))
	{
		$rps=strtr($rps, array('\\'=>'/'));
		while(strlen($rps) && (substr($rps,0,1) == '/'))
			$rps=substr($rps,1);
		while(strlen($rps) && (substr($rps,-1) == '/'))
			$rps=substr($rps,0,-1);
		if(strlen($rps))
			$rps .= '/';
	}

	// Pathes to check
	$ipa=array(0=>'');
	$psa=array('syspath'=>'', 'prgpath'=>'prgdir', 'spppath'=>'sppdir', 'ajxpath'=>'', 'clspath'=>'', 'extpath'=>'');
	foreach($psa as $psp => $psd)
	{
		if(isset($GLOBALS[$psp]))
			$ipa[]=$GLOBALS[$psp];

		if(strlen($rp) && isset($GLOBALS[$rp.'path']))
		{
			$ipa[]=$GLOBALS[$rp.'path'];
			if(strlen($rps))
				$ipa[]=$GLOBALS[$rp.'path'].$rps;
		}

		if(strlen($psd) && fxIsArray($GLOBALS[$psd]))
		{
			if(fxIsArray($GLOBALS['fxpglobals']) && isset($GLOBALS['fxpglobals']['tr']) && is_integer($GLOBALS['fxpglobals']['tr']) && isset($GLOBALS[$psd][$GLOBALS['fxpglobals']['tr']]))
				$ipa[]=$GLOBALS[$psd][$GLOBALS['fxpglobals']['tr']].'/';
			if(sizeof($rpfa))
			{
				foreach($rpfa as $rpf)
				{
					$rpf=(int)$rpf;
					if(isset($GLOBALS[$psd][$rpf]))
						$ipa[]=$GLOBALS[$psd][$rpf].'/';
				}
			}
			if(isset($GLOBALS[$psd][0]))
				$ipa[]=$GLOBALS[$psd][0].'/';
		}
	}

	// Return pathes to check?
	if(!strlen($filename))
		return $ipa;

	// Check for file in pathes and return result
	$filepath='';
	$pfxa=array('.inc', '.php', '.pfi');
	foreach($ipa as $ip)
	{
		foreach($pfxa as $pfx)
		{
			$fn=$filename;
			if(substr($fn,-4) != $pfx)
				$fn .= $pfx;

			$ipfn=$ip.$fn;
			if(file_exists($ipfn))
			{
				$filepath=$ipfn;
				break;
			}
		}
	}
	return $filepath;
}

/**
 * Scan language directory "LNG" for all possible languages - format is "[2-digit Language ID]=[Country Code]=[Language Text].txt", e.g. for English is "07=en=English.txt"
 * The country flags for the languages must be in the "GFX" subdirectory - format is "flg_b[Integer language id].png", e.g. for English is "flg_b7.png"
 *
 * @param boolean $with_lt    - Optional parameter (default = false):  Get language literals for each language from the init file in the LNG subdirectory, e.g. "Language" for English or "Sprache" for German
 * @param boolean $echo_popup - Optional parameter (default = false):  Echo all found languages as string for setting popup
 *
 * @return Language array containg a subarray for each found language id with the informations: "cc"=Country Code, "tx"=Language Text, "pt"=Language Filename, "fl"=Language Flag Path, "lt"=Language Literal
 */
function fxGetAllLanguages($with_lt=false, $echo_popup=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Scan language directory
	$pla=fxScanDir($GLOBALS['lngpath'], false, true, '/[0-9]{2}\=.+\=.+\.txt/');
//fxDebug($pla,'$pla', 0);

	$la=array();
	if(sizeof($pla['files']))
	{
		foreach($pla['files'] as $plfn => $plpn)
		{
			// ...language id, e.g. 7 for English
			$ep=strpos($plfn,'=');
			$plid=(int)substr($plfn,0,$ep);
			$plfn=substr($plfn,$ep+1);
			// ...country code, e.g. "en" for English
			$ep=strpos($plfn,'=');
			$plcc=strtolower(trim(substr($plfn,0,$ep)));
			// ...language text, e.g. "English" for English
			$pltx=ucfirst(trim(substr($plfn,$ep+1,-4)));
			// ...flag (big)
			$plfb=$GLOBALS['gfxpath'].'flg_'.substr('00'.$plid,-2).'b.png';
			if(!file_exists($plfb))
				$plfb=$GLOBALS['gfxpath'].'flg_00b.png';
			// ...flag (small)
			$plfs=$GLOBALS['gfxpath'].'flg_'.substr('00'.$plid,-2).'.png';
			if(!file_exists($plfs))
				$plfs=$GLOBALS['gfxpath'].'flg_00.png';

			$la[$plid]=array('no'=>substr('0'.$plid,-2), 'cc'=>$plcc, 'tx'=>$pltx, 'pt'=>$plpn, 'fl'=>$plfb, 'fs'=>$plfs);

			// ...language literal (from initialization text)
			if($with_lt)
			{
				$itla=fxGetLanguageTexts('init', true, false, $plid, false);
				if(fxIsArray($itla) && isset($itla['hl_lang']))
					$la[$plid]['lt']=$itla['hl_lang'];
				else
					$la[$plid]['lt']='?';
			}
		}
	}
//fxDebug($la,'$la: $lg='.$lg, 0);

	if($echo_popup)
	{
		$str='<div id="langset" style="display:none;">';
		$la_cnt=0;
		foreach($la as $la_id => $laa)
		{
			if($la_cnt)
				$str .= '|';
			$str .= $la_id.'_'.$laa['tx'];
			$la_cnt++;
		}
		$str .= '</div>'.$GLOBALS['nl'];
		echo($str);
	}

	return $la;
}

/**
 * Get language Id
 *
 * @param integer $lang  - Optional parameter (default = 0):     If 0 get current language Id, else check if its a valid language Id
 * @param boolean $check - Optional parameter (default = true):  If true check if it's a valid language, else assume it is
 *
 * @return integrer - Language Id
 */
function fxGetLanguage($lang=0, $check=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$lang=(int)$lang;

	// Check language
	if($check && ($lang > 0))
	{
		$la=fxGetAllLanguages(true);
		if(sizeof($la))
		{
			foreach($la as $lid => $laa)
			{
				if($lang == $lid)
					return $lang;
			}
		}
	}

	// Get language
	$lang=$GLOBALS['fxpglobals']['lang'];
	if(!$lang && isset($GLOBALS['fxpglobals']) && is_array($GLOBALS['fxpglobals']) && isset($GLOBALS['fxpglobals']['dbparam']) && is_array($GLOBALS['fxpglobals']['dbparam']) && isset($GLOBALS['fxpglobals']['dbparam']['language']))
		$lang=(int)$GLOBALS['fxpglobals']['dbparam']['language'];
	if(!$lang)
		$lang=7;	// -> Default 7=English

	return $lang;
}

/**
 * Get current client Id
 *
 * @param integer $cid - Optional parameter (default = 0):  Client ID - if 0 the current client Id is used
 *
 * @return Submitted or current client Id
 */
function fxf_getClientId($cid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Client Id
	$cid=(int)$cid;
	if($cid <= 0)
		$cid=$GLOBALS['fxpglobals']['client'];

	return $cid;
}

/**
 * Create an array with all important client data
 *
 * @param integer $cid - Optional parameter (default = 0):  Client ID - if 0 the current client Id is used
 *
 * @return Array with all client data
 */
function fxf_getClientLanguage($cid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Client Id
	$cid=fxf_getClientId($cid);

	// language
	$lng=0;
	if(db_check('s_einstellungswerte', 't'))
	{
		$sql="SELECT einstellungswert FROM s_einstellungswerte WHERE einstellungs_id=1 AND benutzer_id=1 AND mandanten_id=".$cid;
		$lng=(int)db_value($sql);
		if(!$lng)
		{
			$sql="SELECT einstellungswert FROM s_einstellungswerte WHERE einstellungs_id=1 AND benutzer_id=1 AND mandanten_id=0";
			$lng=(int)db_value($sql);
		}
	}
	if(!$lng)
		$lng=fxGetLanguage(0,false);

	return $lng;
}

/**
 * Get text file or text array for the desired language from the LNG-subdirectory
 *
 * @param various $filename - Mandatory parameter:                   Name of the language file (without the language prefix), e.g. "menu"
 * @param boolean $ra       - Optional parameter (default = true):   Return result as a text array
 * @param boolean $rt       - Optional parameter (default = false):  Return unprocessed text lines
 * @param integer $lang     - Optional parameter (default = 0):      Get file in this language - currently: 1=German + 7=English (0=Language from user setting)
 * @param boolean $check    - Optional parameter (default = true):   If true check if it's a valid language, else assume it is
 *
 * @return Text file or array from the LNG-subdirectory for a specific language
 */
function fxGetLanguageTexts($filename, $ra=true, $rt=false, $lang=0, $check=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Get text in which language
	if(!$lang)
		$lang=fxGetLanguage($lang, $check);

	// Load text file line by line from language directory - Return FALSE if file not found
	$path=$GLOBALS['lngpath'].substr('0'.$lang,-2).'-'.$filename.'.txt';
	if(!file_exists($path))
		return false;
	$tla=fxLoad($path,true);
fxDebug($tla,'$tla: '.$path,2);

	// Init return variable (as array)
	$stla=array();

	// Loop through all lines, parse and insert into the return array
	if(fxIsArray($tla))
	{
		foreach($tla as $line)
		{
			$line=trim($line);

			// Skip empty lines
			if(strlen($line))
			{
				$fc=substr($line,0,1);
				// Skip comments
				if(($fc != '#') && ($fc != '/') && ($fc != ';'))
				{
					if($rt)
						$stla[]=$line;
					else
					{
						// Get type
						$type='';
						if(($fc == '^') || ($fc == '~'))
						{
							$type=$fc;
							$line=substr($line,1);
						}
						// Get separator position
						$sp=strpos($line,':');
						// Skip if no position found
						if($sp > 0)
						{
							$variable=trim(substr($line,0,$sp));
							$text=trim(substr($line,$sp+1));
							// Insert parsed result into return array
							$stla[$variable]=$type.$text;
						}
					}
				}
			}
		}
	}

	// Return result as array or text, depending on the funcion parameter
	// ...array
	if($ra)
		return $stla;
	// ...text
	$stlt='';
	foreach($stla as $id => $tx)
	{
		if($rt)
			$stlt .= $GLOBALS['_divstr'][0].$tx;
		else
			$stlt .= $GLOBALS['_divstr'][0].$id.'|'.$tx;
	}
	return $stlt;
}

/**
 * Handle fx-project session files
 *
 * @param string  $type      - Optional parameter (default = 'g'):    Session file type: g=GLOBALS, t=TRANSACTION DATA + s=SELECTS
 * @param string  $mode      - Optional parameter (default = 'l'):    Mode: l=LOAD, s=SAVE, g=GET, r=READ, d=DELETE, u=UNSET, p=PUT + w=WRITE
 * @param string  $sk        - Optional parameter (default = ''):     Key, e.g. get or write only the value from transaction data with a specific key
 * @param string  $sv        - Optional parameter (default = ''):     Value to be written (only used for mode "p" or "w")
 * @param boolean $unsetform - Optional parameter (default = false):  Unset forms when loading sessions
 * @param boolean $sameform  - Optional parameter (default = false):  Only get session values from the same from
 * @param string  $c         - Optional parameter (default = ''):     Client id (if empty value from current user setting is used)
 * @param string  $p         - Optional parameter (default = ''):     Person id (if empty value from current user setting is used)
 *
 * @return Session value if mode is "g" or "r"
 */
function fxSession($type='g', $mode='l', $sk='', $sv='', $unsetform=false, $sameform=false, $c='', $p='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(isset($GLOBALS['dontsavesessions']) || !strlen($GLOBALS['fxpglobals']['locstoid']) || !strlen($GLOBALS['fxpglobals']['sesstoid']))
		return '';

	$sf=substr(strtolower($type),0,1);	// g=GLOBALS, t=TRANSACTION DATA + s=SELECTS
	$sn=$sf;
	$st='fxpglobals';
	if($sf == 't')
	{
		if(strlen($type) > 1)
			$str=(int)substr($type,1);
		else if(!$GLOBALS['fxpglobals']['user'] || !$GLOBALS['fxpglobals']['tr'])
			$str=0;
		else
			$str=$GLOBALS['fxpglobals']['tr'];
		if($str <= 0)
			$sn='t000';
		else
			$sn='t'.substr('000'.$str,-3);
		$st='fxptdata';
	}
	else if($sf == 's')
		$st='fxpselects';

	$mode=substr(strtolower($mode),0,1);
	if(isset($GLOBALS['dontsavesessions']) && ($mode != 'l'))
		return;
	switch($mode)
	{
		case 'l':	// LOAD
			$sa=array();
			$filename=fxSessionFile($sn.'.fpi',$c,$p);
//fxDebug($filename,'$filename: $sn='.$sn.', $c='.$c.', $p='.$p, 0);
			if(file_exists($filename))
				require($filename);
//fxDebug($sa,'$sa: $filename='.$filename, 0);
			if($sf == 't')
				$GLOBALS['fxptarray']=array();
			if(sizeof($sa))
			{
				if($sf == 't')
					$GLOBALS['fxptarray']=$sa;
				if($sk !== '*load*')
				{
					if(strlen($sk))
						$GLOBALS[$st]=$sa;
					else
					{
						foreach($sa as $sk => $sv)
						{
							$GLOBALS[$st][$sk]=$sv;
							if($sf == 't')	// Transaction
							{
								if($sn == 't000')
									$sameform=true;
								$isform=false;
								if(substr($sk,0,8) == '_fxform_')
									$isform=true;

								// Need certain form?
								if(strlen($GLOBALS['_form']) && $isform)
								{
									if((!$sameform || ($sk == $GLOBALS['_form'])) && fxIsArray($sv))
									{
										foreach($sv as $fsk => $fsv)
										{
											$GLOBALS[$fsk]=$fsv;
											$_POST[$fsk]=$fsv;
										}
									}
								}
								else
								{
									$GLOBALS[$sk]=$sv;
								}

								if($unsetform && $isform)
									unset($GLOBALS[$st][$sk]);
							}
							else
								$GLOBALS[$sk]=$sv;
						}
					}
				}
			}

			// Default globals
			if($sf == 'g')
			{
				$add=0;
				// ...Language
				if(!isset($GLOBALS[$st]['lang']))
				{
					$add++;
					$GLOBALS[$st]['lang']=fxGetLanguage();
				}
				else
				// ...Design
				if(!isset($GLOBALS[$st]['design']))
				{
					$add++;
					$dgds='';
					$vtsl=strtolower(substr($GLOBALS['v_type'],0,1));
					if(strlen($vtsl) && ($vtsl !== 'c'))
					{
						if(file_exists($GLOBALS['csspath'].'01sky.css'))
							$dgds='01sky';
					}
					$GLOBALS[$st]['design']=$dgds;
				}
				// ...Transaction: 0=Start (Login, etc.)
				if(!isset($GLOBALS[$st]['tr']))
				{
					$add++;
					$GLOBALS[$st]['tr']=0;
				}
				// ...Action: 1=Display
				if(!isset($GLOBALS[$st]['action']))
				{
					$add++;
					$GLOBALS[$st]['action']=1;
				}
				// ...Active Filter: 0=Standard
				if(!isset($GLOBALS[$st]['afilter']))
				{
					$add++;
					$GLOBALS[$st]['afilter']='0';
				}
				// ...Client ID
				if(!isset($GLOBALS[$st]['client']))
				{
					$add++;
					$GLOBALS[$st]['client']=0;
				}
				// ...User ID
				if(!isset($GLOBALS[$st]['user']))
				{
					$add++;
					$GLOBALS[$st]['user']=0;
				}
				// ...Sysadmin User
				if(!isset($GLOBALS[$st]['suser']))
				{
					$add++;
					$GLOBALS[$st]['suser']=0;
				}
				// ...Person ID
				if(!isset($GLOBALS[$st]['person']))
				{
					$add++;
					$GLOBALS[$st]['person']=0;
				}
				// ...Project ID
				if(!isset($GLOBALS[$st]['project']))
				{
					$add++;
					$GLOBALS[$st]['project']=0;
				}
				// ...Unit
				if(!isset($GLOBALS[$st]['punit']))
				{
					$add++;
					if(fxIsArray($GLOBALS['fxpglobals']) && fxIsArray($GLOBALS['fxpglobals']['settings']) && $GLOBALS['fxpglobals']['settings']['munit'])
						$GLOBALS[$st]['punit']=$GLOBALS['fxpglobals']['settings']['munit'];
					else
						$GLOBALS[$st]['punit']=FXP_TU_HOUR;	// Hrs.
				}
				// ...Client Name
				if(!isset($GLOBALS[$st]['cname']))
				{
					$add++;
					$GLOBALS[$st]['cname']='';
				}
				// ...User Name
				if(!isset($GLOBALS[$st]['uname']))
				{
					$add++;
					$GLOBALS[$st]['uname']='';
				}
				// ...Person Name
				if(!isset($GLOBALS[$st]['pname']))
				{
					$add++;
					$GLOBALS[$st]['pname']='';
				}
				// ...SSL (Secure-String-Login)
				if(!isset($GLOBALS[$st]['ssl']))
				{
					$add++;
					$GLOBALS[$st]['ssl']='';
				}
				// ...Display Help?
				if(!isset($GLOBALS[$st]['dhelp']))
				{
					$add++;
					$GLOBALS[$st]['dhelp']=true;
				}
				// ...Upload
				if(!isset($GLOBALS[$st]['upload']))
				{
					$add++;
					$GLOBALS[$st]['upload']=array();
				}
				// ...Zoom factor for gantt
				if(!isset($GLOBALS[$st]['zoom']))
				{
					$add++;
					$GLOBALS[$st]['zoom']=6;
				}
				// ...Screen width + height
				if(!isset($GLOBALS[$st]['swidth']))
				{
					$add++;
					if(isset($_GET['swidth']))
						$GLOBALS[$st]['swidth']=(int)$_GET['swidth'];
					else
						$GLOBALS[$st]['swidth']=0;
				}
				if(!isset($GLOBALS[$st]['sheight']))
				{
					$add++;
					if(isset($_GET['sheight']))
						$GLOBALS[$st]['sheight']=(int)$_GET['sheight'];
					else
						$GLOBALS[$st]['sheight']=0;
				}
				// ...Project search test
				if(!isset($GLOBALS[$st]['psearch']))
				{
					$add++;
					$GLOBALS[$st]['psearch']='';
				}
				// ...Backup
				if(!isset($GLOBALS[$st]['backup']))
				{
					$add++;
					$GLOBALS[$st]['backup']=array();
				}

				// Settings
				// ...filter
				if(!isset($GLOBALS[$st]['setfilter']))
				{
					$add++;
					$GLOBALS[$st]['setfilter']=1;
				}
				// ...help
				if(!isset($GLOBALS[$st]['sethelp']))
				{
					$add++;
					$GLOBALS[$st]['sethelp']=1;
				}
				// ...trmark
				if(!isset($GLOBALS[$st]['settrmark']))
				{
					$add++;
					$GLOBALS[$st]['settrmark']=1;
				}
				// ...alignworkday
				if(!isset($GLOBALS[$st]['setalignworkday']))
				{
					$add++;
					$GLOBALS[$st]['setalignworkday']=0;
				}
				// ...showframes
				if(!isset($GLOBALS[$st]['setshowframes']))
				{
					$add++;
					$GLOBALS[$st]['setshowframes']=1;
				}
				// ...showlines
				if(!isset($GLOBALS[$st]['setshowlines']))
				{
					$add++;
					$GLOBALS[$st]['setshowlines']=0;
				}
				// ...contnumbers
				if(!isset($GLOBALS[$st]['setcontnumbers']))
				{
					$add++;
					$GLOBALS[$st]['setcontnumbers']=0;
				}
				// ...showhr
				if(!isset($GLOBALS[$st]['setshowhr']))
				{
					$add++;
					$GLOBALS[$st]['setshowhr']=1;
				}
				// ...ppsppopalways
				if(!isset($GLOBALS[$st]['setppsppopalways']))
				{
					$add++;
					$GLOBALS[$st]['setppsppopalways']=0;
				}
				// ...setwfcolor -> Workflow color mode: 0=B/W, 1=Color
				if(!isset($GLOBALS[$st]['setwfcolor']))
				{
					$add++;
					$GLOBALS[$st]['setwfcolor']=0;
				}
				// ...setwfairow -> Workflow action icon row mode: 0=in 1 row (all 4 icons), 1=in 2 rows (2 icons per row)
				if(!isset($GLOBALS[$st]['setwfairow']))
				{
					$add++;
					$GLOBALS[$st]['setwfairow']=0;
				}

				// Transaction settings
				if(!isset($GLOBALS[$st]['trsettings']))
				{
					$add++;
					$GLOBALS[$st]['trsettings']=array(
						189	=> array(
							'wlscbl'	=> 0,
							'wlscba'	=> 1,
							'wlscbp'	=> 0,
							'wlscbs'	=> 1,
							'wlsras'	=> 0
						)
					);
				}

				// Save changes?
				if($add > 0)
					fxSession($sf,'s');
			}
		break;

		case 's':	// SAVE
			$filename=fxSessionFile($sn.'.fpi',$c,$p);
			if(strlen($filename))
			{
				$fex=false;
				if(file_exists($filename))
					$fex=true;

				$sta=array('g'=>'GLOBALS', 't'=>'TRANSACTION DATA (= PROGRAM FUNCTION DATA)', 's'=>'SELECTS');

				$inc  = '<'.'?'.'php'.$GLOBALS['nl_b'];
				$inc .= '// This is a ['.$sta[$sn].'] session file created by fx-project.'.$GLOBALS['nl_b'];
				$inc .= '$sa=array('.$GLOBALS['nl_b'];
				if(sizeof($GLOBALS[$st]))
				{
					$cnt=1;
					$soa=sizeof($GLOBALS[$st]);
					foreach($GLOBALS[$st] as $gk => $gv)
					{
						if(($gk != 'locstoid') && ($gk != 'sesstoid') && ($gk != 'mainpath'))
						{
							$inc .= fxWriteVar($gk,$gv,1);
							if($cnt < $soa)
								$inc .= ','.$GLOBALS['nl_b'];
						}
						$cnt++;
					}
				}
				$inc .= $GLOBALS['nl_b'].');'.$GLOBALS['nl_b'];
				$inc .= '?'.'>';

				if($fex)
					fxDelete($filename);

				fxSave($filename, $inc, true);

				// Person globals
				if($type == 'g')
				{
					if(strlen($c))
						$c=(int)$c;
					else
						$c=$GLOBALS['fxpglobals']['client'];
					if(strlen($p))
						$p=(int)$p;
					else
						$p=$GLOBALS['fxpglobals']['person'];
					if($c > 0)
					{
						$gcp_filename=fxSessionFile('g.fpi',$c,$p,true);
						if(strlen($gcp_filename))
						{
							fxDelete($gcp_filename);
							@copy($filename, $gcp_filename);
						}
					}
				}
			}
		break;

		case 'g':	// GET
		case 'r':	// READ
		case 'd':	// DELETE
		case 'u':	// UNSET
			$rv='';
			$pf=false;
			$sk=trim($sk);
			if(sizeof($GLOBALS[$st]))
			{
				if(strlen($sk))
				{
					// Form variable?
					if(strlen($GLOBALS['_form']) && ($sf == 't'))
						$sk=$GLOBALS['_form'].'['.$sk.']';
					$ska=explode('[',$sk);
					$cnt=1;
					$skl=sizeof($ska);
					$ap=&$GLOBALS[$st];
					foreach($ska as $ssk)
					{
						while(substr($ssk,-1) == ']')
							$ssk=substr($ssk,0,-1);
						if((substr($ssk,0,1) == '"') || (substr($ssk,0,1) == "'"))
							$ssk=substr($ssk,1);
						if((substr($ssk,-1) == '"') || (substr($ssk,-1) == "'"))
							$ssk=substr($ssk,0,-1);
						if((string)((int)$ssk) == (string)$ssk)
							$ssk=(int)$ssk;
						if($cnt == $skl)
						{
							if(($mode == 'g') || ($mode == 'r'))
								$rv=$ap[$ssk];
							else
							{
								unset($ap[$ssk]);
								$pf=true;
							}
						}
						else if(is_array($ap) && sizeof($ap) && isset($ap[$ssk]))
							$ap=&$ap[$ssk];
						else
							break;
						$cnt++;
					}
				}
				else if(($mode == 'd') || ($mode == 'u'))
				{
					if($sf == 'g')
						$na=array('locstoid'=>$GLOBALS[$st]['locstoid'], 'sesstoid'=>$GLOBALS[$st]['sesstoid'], 'mainpath'=>$GLOBALS[$st]['mainpath']);
					else
						$na=array();
					$GLOBALS[$st]=$na;
					$pf=true;
				}
			}

			// GET or READ
			if(($mode == 'g') || ($mode == 'r'))
			{
				return fxDecodeValue($rv);
			}

			// DELETE
			if($pf && ($mode == 'd'))
				fxSession($sf,'s');
		break;

		case 'p':	// PUT
		case 'w':	// WRITE
			$sk=trim($sk);
			if(strlen($sk))
			{
				// Form variable?
				if(strlen($GLOBALS['_form']) && ($sf == 't'))
					$sk=$GLOBALS['_form'].'['.$sk.']';
				$ska=explode('[',$sk);
				$cnt=1;
				$skl=sizeof($ska);
				$ap=&$GLOBALS[$st];
				foreach($ska as $ssk)
				{
					while(substr($ssk,-1) == ']')
						$ssk=substr($ssk,0,-1);
					if((substr($ssk,0,1) == '"') || (substr($ssk,0,1) == "'"))
						$ssk=substr($ssk,1);
					if((substr($ssk,-1) == '"') || (substr($ssk,-1) == "'"))
						$ssk=substr($ssk,0,-1);
					if((string)((int)$ssk) == (string)$ssk)
						$ssk=(int)$ssk;
					if($cnt == $skl)
					{
						$ap[$ssk]=$sv;
					}
					else
					{
						if(!is_array($ap) || !sizeof($ap) || !isset($ap[$ssk]))
							$ap[$ssk]=array();
						$ap=&$ap[$ssk];
					}
					$cnt++;
				}
				// WRITE
				if($mode == 'w')
					fxSession($sf,'s');
			}
		break;
	}
}

/**
 * Get the session filename for a specific type
 *
 * @param string  $sn  - Optional parameter (default = 'g'):    Session file type: g=GLOBALS, t=TRANSACTION DATA + s=SELECTS
 * @param string  $c   - Optional parameter (default = ''):     Client id (if empty value from current user setting is used)
 * @param string  $p   - Optional parameter (default = ''):     Person id (if empty value from current user setting is used)
 * @param boolean $gcp - Optional parameter (default = false):  Get client and person specific filename
 *
 * @return Session filename for a specific type
 */
function fxSessionFile($sn='g', $c='', $p='', $gcp=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(isset($GLOBALS['dontsavesessions']) || !strlen($GLOBALS['fxpglobals']['locstoid']) || !strlen($GLOBALS['fxpglobals']['sesstoid']))
		return '';

	$sf=substr(strtolower($sn),0,1);	// g=GLOBALS, t=TRANSACTION + s=SELECTS
	if(strlen($c))
		$c=(int)$c;
	else
		$c=$GLOBALS['fxpglobals']['client'];
	if(strlen($p))
		$p=(int)$p;
	else
		$p=$GLOBALS['fxpglobals']['person'];

	$filename=$GLOBALS['usrpath'];
	if($gcp || (($sf == 't') && (substr($sn,0,4) != 't000') && ($c > 0) && !isset($GLOBALS['insync'])))
	{
		$filename .= 'C'.substr('0000'.$c,-4).'/';
		if(!is_dir($filename))
			@mkdir($filename, 0777);

		$filename .= 'P'.substr('000000000'.$p,-9).'/';
		if(!is_dir($filename))
			@mkdir($filename, 0777);
	}
	else if(strlen($GLOBALS['fxpglobals']['locstoid']))
	{
		$filename .= $GLOBALS['fxpglobals']['locstoid'].'/';
		if(!is_dir($filename))
			@mkdir($filename, 0777);
	}

	$ppath=$filename;

	if(!$gcp && strlen($GLOBALS['fxpglobals']['locstoid']) && strlen($GLOBALS['fxpglobals']['sesstoid']))
	{
		$flts=$GLOBALS['fxpglobals']['sesstoid'];
		$filename .= $flts.'/';
		if(!file_exists($filename))
		{
			// Get directory structure
			$dsa=fxScanDir($ppath,true,false);
//fxDebug($dsa,'$dsa', 0);

			// Get latest and outdated directory names
			$sf_ln='';
			$sf_oa=array();
			if($dsa['dir_entries'] > 0)
			{
				$sf_mn='S'.date('Ymd').'000000';	// Today's minimum session name
				foreach($dsa['dirs'] as $pfn => $pfp)
				{
					// Session directory?
					if((substr($pfn,0,1) == 'S') && (strlen($pfn) >= 15))
					{
						// Latest?
						if(!strlen($sf_lp) || (strcmp($pfn,$sf_ln) > 0))
							$sf_ln=$pfn;
						// Outdated?
						if(strcmp($pfn,$sf_mn) < 0)
							$sf_oa[$pfn]=$pfp;
					}
				}
			}
//fxDebug($sf_ln,'$sf_ln', 0); fxDebug($sf_oa,'$sf_oa', 0);

			// Create new session directory
			@mkdir($filename, 0777);
			$sf_ex=file_exists($filename);

			// Copy every file from latest to new session directory
			if($sf_ex && strlen($sf_ln))
			{
				// Get files in directory
				$sf_lp=$dsa['dirs'][$sf_ln];
				$fsa=fxScanDir($sf_lp,false);
//fxDebug($fsa,'$fsa: $sf_lp='.$sf_lp, 0);
				if($fsa['file_entries'] > 0)
				{
					foreach($fsa['files'] as $ffn => $ffp)
					{
//echo('Copy latest session file &quot;'.$ffp.'&quot; to new session directory: '.$filename.'<br />');
						@copy($ffp, $filename.$ffn);
					}
				}
			}

			// Delete outdated session directories
			if($sf_ex && sizeof($sf_oa))
			{
				foreach($sf_oa as $ofn => $ofp)
				{
					$delstat=fxDelete($ofp, '', 0, true);
//echo('Delete outdated session directory &quot;'.$ofp.'&quot;... Status='.$delstat.'<br />');
				}
			}
//die;
		}
	}

	// We don't have our session paths (yet), return empty string
	$sp=$GLOBALS['usrpath'].$GLOBALS['fxpglobals']['locstoid'].'/';
	if(!file_exists($sp))
		return '';
	$sp .= $GLOBALS['fxpglobals']['sesstoid'].'/';
	if(!file_exists($sp))
		return '';

	// Return path and filename
	$filename .= $sn;
//echo('$sn='.$sn.', $filename='.$filename.'<br />');
	return $filename;
}

/**
 * Convert a variable to text
 *
 * @param various $gk   - Mandatory parameter:               Key/Name of the variable
 * @param various $gv   - Mandatory parameter:               Value of the variable
 * @param integer $tabs - Optional parameter (default = 0):  Tabulators to use as a prefix
 *
 * @return Converted text
 */
function fxWriteVar($gk,$gv,$tabs=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sca=fxReplaceSpecialCharacters(false,true,true,false,true);

	// Tabulators
	$tv='';
	if($tabs > 0)
		$tv=str_repeat("\t",$tabs);
	$rv=$tv;

	// Key
	$arv='=>';
	if(is_string($gk))
	{
		if(substr($gk,0,1) == '=')
		{
			$rv .= substr($gk,1);
			$arv='';
		}
		else
			$rv .= '\''.strtr($gk,$sca).'\'';
	}
	else
		$rv .= $gk;
	$rv .= $arv;

	// Value
	if(is_string($gv))
		$rv .= '\''.strtr($gv,$sca).'\'';
	else if(is_bool($gv))
	{
		if($gv)
			$rv .= 'true';
		else
			$rv .= 'false';
	}
	else if(is_array($gv))
	{
		if(sizeof($gv))
		{
			$rv .= 'array('.$GLOBALS['nl_b'];
			$cnt=1;
			$soa=sizeof($gv);
			foreach($gv as $gsk => $gsv)
			{
				$rv .= fxWriteVar($gsk,$gsv,$tabs+1);
				if($cnt < $soa)
					$rv .= ','.$GLOBALS['nl_b'];
				$cnt++;
			}
			$rv .= $GLOBALS['nl_b'].$tv.')';
		}
		else
			$rv .= 'array()';
	}
	else if(strlen($gv))
		$rv .= $gv;
	else
		$rv .= 'NULL';

	return $rv;
}

/**
 * Encode a string
 *
 * @param various $rv - Mandatory parameter:  String to be encoded
 *
 * @return Encoded string
 */
function fxEncodeValue($rv)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($rv))
	{
		$srv=strlen($rv);
		$o=rand(5,150);
		$ev='*#'.substr('000'.$o,-3);
		for($i=0;$i<$srv;$i++)
		{
			$ev .= substr('000'.(ord(substr($rv,$i,1))+$o),-3);
			$o += 3;
		}
		$ev .= '#*';

		return $ev;
	}
	return $rv;
}

/**
 * Decode an encoded string
 *
 * @param various $rv - Mandatory parameter:  Encoded string to be decoded
 *
 * @return Decoded string
 */
function fxDecodeValue($rv)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($rv))
	{
		// Password?
		$srv=strlen($rv);
		if(($srv > 6) && (substr($rv,0,2) == '*#') && (substr($rv,-2) == '#*'))
		{
			// *#010 075 079 083 #*	=> 5,8,11 - 16
			$o=(int)substr($rv,2,3);	// Offset (position 2-4)
			$nrv='';
			for($i=5; $i<$srv-4; $i+=3)
			{
				$nrv .= chr((int)substr($rv,$i,3)-$o);
				$o += 3;
			}
			$rv=$nrv;
		}
	}
	return $rv;
}

/**
 * Replace special problematic characters in a string
 *
 * @param various $str      - Mandatory parameter:                   Source string
 * @param boolean $tosymbol - Optional parameter (default = true):   Replace special characters to readable symbols, e.g. "<" to "((lt))"
 * @param boolean $extended - Optional parameter (default = false):  Use extended smybol conversion, e.g. also replace apostrophes and backslashes
 * @param boolean $usehtml  - Optional parameter (default = false):  Use html syntax, e.g. "&lt;" for "<"
 * @param boolean $forfile  - Optional parameter (default = false):  Replace string for use as a filename
 *
 * @return Converted string
 */
function fxReplaceSpecialCharacters($str, $tosymbol=true, $extended=false, $usehtml=false, $forfile=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($usehtml)
	{
		$nbsp='&nbsp;'; $lt='&lt;'; $gt='&gt;'; $euro='&euro;'; $and='&amp;'; $quot='&quot;';
	}
	else
	{
		$nbsp=chr(160); $lt='<'; $gt='>'; $euro='€'; $and='&'; $quot='"';
	}

	if($forfile)
	{
		$apos='\\\''; $backslash='\\\\';
	}
	else
	{
		$apos='\''; $backslash='\\';
	}

	if($tosymbol)
	{
		$conv_array=array(
			'((nbsp))'=>$nbsp,	'((lt))'=>$lt,	'((gt))'=>$gt,	'((and))'=>$and,	'((quot))'=>$quot,	'((plus))'=>'+',	'((equal))'=>'=',	'((backslash))'=>$backslash,
			'((euro))'=>$euro,	chr(37).chr(117).chr(50).chr(48).chr(65).chr(67)=>$euro,
			'((apos))'=>$apos,	chr(37).chr(117).chr(50).chr(48).chr(51).chr(50)=>$apos
		);

		if($extended)
		{
			$conv_array['\'']=$apos;
			$conv_array['\\']=$backslash;
		}
	}
	else
	{
		$conv_array=array(
			'&middot;'=>'°',	'&nbsp;'=>chr(160),	'<'=>'((lt))',		$lt=>'((lt))',		'>'=>'((gt))',		$gt=>'((gt))',
			'€'=>'((euro))',	$euro=>'((euro))',	'&'=>'((and))',		$and=>'((and))',	'"'=>'((quot))',	$quot=>'((quot))',
			'+'=>'((plus))',	'='=>'((equal))',	'\''=>'((apos))',	'\\'=>'((backslash))'
		);
	}

	if(is_string($str))
		return strtr($str, $conv_array);

	return $conv_array;
}

/**
 * Display html "select" as "ul" that can be handled by our javascript routine
 *
 * @param various $id  - Mandatory parameter:  Id of the "select" to be displayed
 * @param various $sel - Mandatory parameter:  Selected value
 */
function drawSelect($id,$sel)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Load selects
	fxSession('s','l');

	if(!fxIsArray($GLOBALS['fxpselects'][$id]))
	{
		$multi=strpos($id,'[');
		if($multi)
			$id=substr($id,0,$multi);
	}

	$sa=array();
	if(fxIsArray($GLOBALS['fxpselects'][$id]))
	{
		$sa=$GLOBALS['fxpselects'][$id];
		$valids=getValidSelects($id);
		$ta=array('€'=>'&euro;');
	}

	$md='';
	if(strlen($GLOBALS['fxpglobals']['browsermobile']))
		$md=' '.fxf_jsFunction('#mouseDown');

	echo('<ul>');
	$sc=1;
	foreach($sa as $sv => $st)
	{
		$sv=trim($sv);
		if(strlen($sv) && (!sizeof($valids) || isset($valids[$sv])))
		{
			$ca='';
			if($sv == $sel)
				$ca='sel';

			if(isset($valids[$sv]) && is_string($valids[$sv]) && strlen($valids[$sv]))
				$st=$valids[$sv];
			$st=strtr(trim($st),$ta);

			echo('<li id="fspo-'.$sc.'" class="sentry'.$ca.'" svalue="'.$sv.'"'.$md.'>'.$st.'</li>');
			$sc++;
		}
	}
	if($sc == 1)
		echo('<li id="fspo-'.$sc.'" class="sentry" svalue="0" style="background:#ecb5bb;"'.$md.'>?: '.$id.'</li>');
	echo('</ul>');
}

/**
 * Get valid select keys
 *
 * @param various $id - Mandatory parameter:  Id of the "select" to be checked
 *
 * @return Array that contains all the valid keys for the checked "select"
 */
function getValidSelects($id)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$valids=array();

	// Absent type in transaction 222: Absent
	if(($id == 'Fehltagsart') && ($GLOBALS['fxpglobals']['tr'] == 222))
	{
		// Get selected person
		fxSession('t','l','*load*');
		$pid=$GLOBALS['fxptarray']['_fxform_datmask_'.$GLOBALS['fxpglobals']['action']]['Personen_ID'];

		// Get selected appointment type
		$avt=(int)$GLOBALS['fxptarray']['avt'];

		// Get person type
		$ptyp=(int)$GLOBALS['fxpglobals']['persdat']['personentyp'];

		// Get person rights
		$allrights=(int)$GLOBALS['fxptarray']['allrights'];

		// Setting, if contractors can have the same absent types like employees
		$cont_like_emp=(int)$GLOBALS['fxptarray']['cont_like_emp'];

		// Setting, if administration can also approve apsent requests for persons who have a different supervisor
		$ap_can_appr_all=(int)$GLOBALS['fxptarray']['ap_can_appr_all'];

		// Get all contractors
		$contractors_arr=csv_to_array($GLOBALS['fxptarray']['contractors']);

		// Get all subordinates
		$subordinates_arr=csv_to_array($GLOBALS['fxptarray']['subordinates']);

		// Get all persons without a supervisor
		$nosupervisors_arr=csv_to_array($GLOBALS['fxptarray']['nosupervisors']);

		// Get all absent types
		$absenttypes_arr=csv_to_array($GLOBALS['fxptarray']['absenttypes']);

		// Contractor?
		$is_contractor=0;
		if(fxIsArray($contractors_arr))
		{
			foreach($contractors_arr as $apid => $dummy)
			{
				if($apid == $pid)
				{
					$is_contractor=1;
					break;
				}
			}
		}

		// Subordinate?
		$is_subordinate=0;
		if(fxIsArray($subordinates_arr))
		{
			foreach($subordinates_arr as $apid => $dummy)
			{
				if($apid == $pid)
				{
					$is_subordinate=1;
					break;
				}
			}
		}

		// Supervisor?
		$has_no_supervisor=0;
		if(fxIsArray($nosupervisors_arr))
		{
			foreach($nosupervisors_arr as $apid => $dummy)
			{
				if($apid == $pid)
				{
					$has_no_supervisor=1;
					break;
				}
			}
		}

		if(fxIsArray($absenttypes_arr))
		{
			foreach($absenttypes_arr as $atype => $dummy)
			{
				// Check, if absence type is valid for the selected person
				$valid=1;
				if((($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)) && ($atype != $avt))
				{
					// Employees cannot have the absent type:
					// 2879: "Absent"
					if(!$is_contractor && ($atype == 2879))
						$valid=0;

					// Contractors cannot have leave applications, except if client setting allows it:
					// 1178: "Leave application", 1039: "Leave", 2877: "Special leave application" and 2878: "Special leave"
					if(($valid === 1) && $is_contractor && ($cont_like_emp == 0) && (($atype == 1178) || ($atype == 1039) || ($atype == 2877) || ($atype == 2878)))
						$valid=0;

					// Contractors cannot have overtime outpayments:
					// 1942: "Overtime outpayment application" and 1943: "Overtime outpayment"
					if(($valid === 1) && $is_contractor && (($atype == 1942) || ($atype == 1943)))
						$valid=0;

					// Directing managers don't have to apply for appointments in general...
					// Supervisors don't have to apply for appointments of their subordinates...
					// Administrative personal don't have to apply for an appointment of persons if set for the client or has no supervisor...
					// ...therefore cannot have the types:
					// 1178: "Leave application", 2877: "Special leave application", 2105: "Sickness apllication", 2106: "Training application", 1940: "Flextime application" and 1942: "Overtime outpayment application"
					if(($valid === 1) && (($atype == 1178) || ($atype == 2877) || ($atype == 2105) || ($atype == 2106) || ($atype == 1940) || ($atype == 1942)))
					{
						if($allrights == 2)
							$valid=0;
						else if($is_subordinate)
							$valid=0;
						else if(($allrights == 1) && (($ap_can_appr_all && ($pid != $GLOBALS['fxpglobals']['person'])) || $has_no_supervisor))
							$valid=0;
					}

					// Persons with standard rights have to apply for an appointment...
					// Administrative personal is opposit from applications...
					// ...therefore cannot have the types:
					// 1039: "Leave", 2878: "Special leave", 1040: "Sickness", 1468: "Training", 1941: "Flextime" and 1943: "Overtime outpayment"
					if(($valid === 1) && (($atype == 1039) || ($atype == 2878) || ($atype == 1040) || ($atype == 1468) || ($atype == 1941) || ($atype == 1943)))
					{
						$valid=0;
						if($allrights == 2)
							$valid=1;
						else if($is_subordinate)
							$valid=1;
						else if(($allrights == 1) && (($ap_can_appr_all && ($pid != $GLOBALS['fxpglobals']['person'])) || $has_no_supervisor))
							$valid=1;
					}
				}

				if($valid === 1)
					$valids[(int)$atype] = true;
//$valids[(int)$atype] = $atype.': pid='.$pid.', ptyp='.$ptyp.', allrights='.$allrights.', cont_like_emp='.$cont_like_emp.', is_contractor='.$is_contractor.', is_subordinate='.$is_subordinate.', has_no_supervisor='.$has_no_supervisor.' -- valid='.$valid;
			}
		}
	}

	return $valids;
}

/**
 * Get and display selected value of a "select"
 *
 * @param various $id - Mandatory parameter:  Id of the "select" to be checked
 * @param various $sk - Mandatory parameter:  Key of the selected entry
 */
function getSelectedValue($id,$sk)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Load selects
	fxSession('s','l');
//fxDebug($GLOBALS['fxpselects'],'fxpselects: $id='.$id.', $sk='.$sk, 0);

	if(!fxIsArray($GLOBALS['fxpselects'][$id]))
	{
		$multi=strpos($id,'[');
		if($multi)
			$id=substr($id,0,$multi);
	}

	$sv='?';
	$fk='';
	if(fxIsArray($GLOBALS['fxpselects'][$id]))
	{
		$ta=array('€'=>'&euro;');
		$valids=getValidSelects($id);

		if(isset($GLOBALS['fxpselects'][$id][$sk]))
		{
			$sv=trim($GLOBALS['fxpselects'][$id][$sk]);
			if(strlen($sv) && (!sizeof($valids) || isset($valids[$sk])))
			{
				if(isset($valids[$sk]) && is_string($valids[$sk]) && strlen($valids[$sk]))
					$sv=trim($valids[$sk]);
				$sv=strtr($sv,$ta);
			}
			else
				$sv='?';
		}

		if($sv === '?')
		{
			// Report column select
			if((substr($id,0,7) == 'rcmain_') || (substr($id,0,6) == 'rcsel_'))
			{
				foreach($GLOBALS['fxpselects'][$id] as $k => $v)
				{
					$ke=explode('_',$k);
					if($ke[1] === $sk)
					{
						$fk='|'.$k;
						$sv=strtr(trim($v),$ta);
						break;
					}
				}
			}
			// First valid element
			else
			{
				foreach($GLOBALS['fxpselects'][$id] as $k => $v)
				{
					if(strlen($v) && (!sizeof($valids) || isset($valids[$k])))
					{
						$sv=trim($v);
						if(isset($valids[$k]) && is_string($valids[$k]) && strlen($valids[$k]))
							$sv=trim($valids[$sk]);
						$sv=strtr($sv,$ta);
						$fk='|'.$k;
						break;
					}
				}
			}
		}

		$u=0;
		if(($sv === '?') && ((substr($id,0,7) == 'rcmain_') || (substr($id,0,6) == 'rcsel_')))
		{
			foreach($GLOBALS['fxpselects'][$id] as $k => $v)
			{
				if($v === '?')
				{
					fxSession('s','u',$id.'['.$k.']');
					$u++;
				}
			}

			$sk='x_'.$sk;
			$fk='|'.$sk;
			fxSession('s','p',$id.'['.$sk.']',$sv);
			$u++;
		}

		if($u)
			fxSession('s','s');
	}

	echo($sv.$fk);
}

/**
 * Toolbar: Display Top 20 and Last 20 favorites
 */
function drawFavorites()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$l=20;
	$ma=loadMenu();
	if(fxIsArray($ma))
	{
		echo('<table border="0" cellpadding="0" cellspacing="6">');
		echo('	<tr>');
		echo('		<td align="center"><b>Last '.$l.'</b></td>');
		echo('		<td align="center"><b>Top '.$l.'</b></td>');
		echo('	</tr>');
		echo('	<tr>');
		// Last transactions
		echo('		<td nowrap>');
		if(fxIsArray($GLOBALS['fxpglobals']['trstack']))
		{
			$c=0;
			foreach($GLOBALS['fxpglobals']['trstack'] as $ltr)
			{
				if($c < $l)
				{
					$ltra=explode('_',$ltr);
					$rtr=$ltra[0];
					$arg='';
					if(sizeof($ltra) > 1)
						$arg=$ltra[1];
					foreach($ma as $tk => $tc)
					{
						if(($tc['tr'] == $rtr) && ($tc['ar'] == $arg))
						{
							if(strlen($arg))
								$arg=' arg="'.$arg.'"';
							echo('<div id="fxm_'.$tk.'_l" class="favlink" tr="'.$rtr.'"'.$arg.' tooltip="'.$tc['tt'].'" style="margin-top:4px;cursor:pointer;">&nbsp;'.$tc['tx'].'&nbsp;</div>');
							$c++;
						}
					}
				}
			}
		}
		echo('		</td>');
		// Favorite
		echo('		<td nowrap>');
		if(fxIsArray($GLOBALS['fxpglobals']['trcounter']))
		{
			$st=array();
			foreach($GLOBALS['fxpglobals']['trcounter'] as $ltr => $tct)
				$st[substr('000000000'.$tct,-9).'-'.substr('0000'.$ltr,-4)]=$ltr;
			krsort($st);
			$c=0;
			foreach($st as $id => $ltr)
			{
				if($c < $l)
				{
					$ltra=explode('_',$ltr);
					$rtr=$ltra[0];
					$arg='';
					if(sizeof($ltra) > 1)
						$arg=$ltra[1];
					foreach($ma as $tk => $tc)
					{
						if(($tc['tr'] == $rtr) && ($tc['ar'] == $arg))
						{
							if(strlen($arg))
								$arg=' arg="'.$arg.'"';
							echo('<div id="fxm_'.$tk.'_l" class="favlink" tr="'.$rtr.'"'.$arg.' tooltip="'.$tc['tt'].'" style="margin-top:4px;cursor:pointer;">&nbsp;'.$tc['tx'].'&nbsp;</div>');
							$c++;
						}
					}
				}
			}
		}
		echo('		</td>');
		echo('	</tr>');
		echo('</table>');
	}
}

/**
 * Toolbar: Display search result
 *
 * @param various $search - Mandatory parameter:  String to be searched
 */
function drawSearch($search)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$vt=array();
	// Load authorized program functions
	$ma=loadMenu();
	if(fxIsArray($ma))
	{
		foreach($ma as $tk => $tc)
		{
			if($tc['tr'])
				$vt[$tc['tr']]=true;
		}
	}
//fxDebug($vt,'$vt');

	// Load all possible program functions?
	if(strtolower(substr($GLOBALS['fxpglobals']['settings']['alle_tr_anzeig'],0,1)) !== 'n')
		$ma=loadMenu(1,false);
	if(fxIsArray($ma))
	{
		echo('<div id="searchpopc" style="position:absolute;left:4px;top:4px;right:4px;bottom:4px;overflow:auto;">');

		$name_pattern='';
		$replace='';
		if(strlen($search))
		{
			$l=strtolower($search);
			if(($l != 'suche') && ($l != 'search'))
			{
				$name_pattern='/'.preg_quote($l,'/').'/i';
				$replace="<b {id} class=\"red\">$0</b>";
			}
			else
				$search='';
		}
//echo('$search='.$search.'<br />$name_pattern='.$name_pattern.'<br />$replace='.$replace.'<hr />');

		// Prepare search parameters
		$sys=true;
		if(!$GLOBALS['fxpglobals']['person'])
			$sys=false;
		foreach($ma as $tk => $tc)
		{
			$ma[$tk]['mt']=false;
			$ma[$tk]['disp']=false;
			$ma[$tk]['valid']=false;
			if($sys && !$tc['lv'] && ($tc['tx'] != 'Sysadmin'))
				$sys=false;
			$ma[$tk]['sys']=$sys;
			if(!$sys && $tc['tr'])
			{
				$col='cccccc';
				if(isset($vt[$tc['tr']]))
				{
					$ma[$tk]['valid']=true;
					$ntk=substr($tk,0,-3);
					while(strlen($ntk) > 1)
					{
						$ma[$ntk]['valid']=true;
						$ntk=substr($ntk,0,-3);
					}
					$col='bbbbbb';
				}
//				$ma[$tk]['tx']='<font {id} color="#'.$col.'">'.substr('00'.$tc['tr'],-3).':</font> '.$tc['tx'];
				$ma[$tk]['fn']='<font {id} color="#'.$col.'">';
				$ma[$tk]['tn']=substr('00'.$tc['tr'],-3);
				$ma[$tk]['tx']=$tc['tx'];
			}
		}
		// Set search filter
		foreach($ma as $tk => $tc)
		{
			if($tc['sys'])
				continue;

			if(strlen($search))
			{
				$mtx=preg_match($name_pattern,$ma[$tk]['tx']);
				if($mtx)
				{
					$ma[$tk]['mt']=true;
					$ma[$tk]['tx']=preg_replace($name_pattern,$replace,$ma[$tk]['tx']);
				}
				$mtn=preg_match($name_pattern,$ma[$tk]['tn']);
				if($mtn)
				{
					$ma[$tk]['mt']=true;
					$ma[$tk]['tn']=preg_replace($name_pattern,$replace,$ma[$tk]['tn']);
				}
				if($mtx || $mtn)
					$ma[$tk]['disp']=true;
			}
			else
				$ma[$tk]['disp']=true;

			if($ma[$tk]['disp'])
			{
//echo('DISPLAY '.$tk.': '.$ma[$tk]['tx'].'<br />');
				// Display all parents if transaction is displayed
				if(strlen($tk) > 2)
				{
					$ptk=substr($tk,0,-3);
					while(strlen($ptk) > 1)
					{
//echo('...DISPLAY PARENT '.$ptk.': '.$ma[$ptk]['tx'].'<br />');
						$ma[$ptk]['disp']=true;
						$ptk=substr($ptk,0,-3);
					}
				}

				// Display all children if folder matches pattern
				if(!$ma[$tk]['tr'] && $ma[$tk]['mt'])
				{
					$ltk=strlen($tk);
					foreach($ma as $ctk => $ctc)
					{
						if(substr($ctk,0,$ltk) == $tk)
						{
//echo('...DISPLAY CHILD '.$ctk.': '.$ma[$ctk]['tx'].'<br />');
							$ma[$ctk]['disp']=true;
						}
					}
				}
			}
		}

		// Delete hidden entries
		foreach($ma as $tk => $tc)
		{
			if(!$tc['disp'])
			{
//echo('DELETE: '.$tk.' - '.$tc['tx'].'<br />');
				unset($ma[$tk]);
			}
		}

		if(fxIsArray($ma))
		{
			$c=0;
			foreach($ma as $tk => $tc)
			{
				if($c && !$tc['lv'])
					echo('<br />');

				if($tc['tr'])
				{
					$tx=$tc['fn'].$tc['tn'].':</font> '.$tc['tx'];

					$arg='';
					if(strlen($tc['ar']))
						$arg=' arg="'.$tc['ar'].'"';
					$id='id="fxm_'.substr($tk,1).'_l" tr="'.$tc['tr'].'"'.$arg;

					if($tc['valid'])
						$cls='favlink';
					else
						$cls='favinvalid';

					$str=str_replace('{id}',$id,'<div {id} class="'.$cls.'" style="margin-left:'.($tc['lv']*20).'px;margin-top:4px;cursor:pointer;">'.$tx.'</div>');
				}
				else
				{
					$tx=$tc['tx'];

					$id='id="fxm_'.substr($tk,1).'_l"';
					if($tc['valid'])
						$col='000000';
					else
						$col='777777;font-style:italic';
					$str=str_replace('{id}',$id,'<div {id} style="margin-left:'.($tc['lv']*20).'px;margin-top:4px;color:#'.$col.';text-decoration:underline;">'.$tx.'</div>');
				}
				echo($str);
				$c++;
			}
		}
		echo('</div>');
	}
}

/**
 * Load and/or create text menu structure and convert to array
 * Save the result for each client to speed up things
 *
 * @param boolean $all      - Optional parameter (default = false):  Get all menu entries (true) or just the authorized entries (false)
 * @param boolean $hide     - Optional parameter (default = true):   Hide unauthorized entries (true) or display them unclickable (false)
 * @param boolean $recreate - Optional parameter (default = false):  Recreate the client's menu file
 *
 * @return Converted menu array
 */
function loadMenu($all=false,$hide=true,$recreate=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

//$recreate=true;
	if(!$GLOBALS['fxpglobals']['client'] && !$all)
		return array();

	// Exists global menu
	$ca=fxf_getClientFilenames();
	$textmenu=$ca['tmenu'];
	$gmenu=$ca['gmenu'];
//echo('Check global menu: $textmenu='.$textmenu.', $gmenu='.$gmenu.'<br />');
	// ...No, create it
	if(file_exists($textmenu) && (!file_exists($gmenu) || $recreate))
	{
		$ca=array();

		$ft='';
		$mg='';
		$rg='';
		$rr='';

//echo('Load text menu: $filepath='.$filepath.', $textmenu='.$textmenu.'<br />');
		$fp=@fopen($textmenu, 'r');
		if($fp)
		{
			$ft=fread($fp, filesize($textmenu));
			fclose($fp);
		}

		if(strlen($ft))
		{
//echo('<pre>'.fxHtmlEncode($ft).'</pre>');
			$fe=explode("\n", $ft);
			// First, get all program function tooltips / descriptions
			$tda=array();
			foreach($fe as $line)
			{
				$tline=rtrim(str_replace("\r", '', $line));
				$line=ltrim($tline);
				if(strlen($line) && (substr($line,0,1) == '>'))
				{
					// tr=Transaction number | tt=Tooltip (Menu) | dw=Description (Workflow)
					$la=explode('|',$line);
					if(sizeof($la) > 1)
					{
						$pf=(int)substr($la[0],1);
						if($pf)
						{
							$tda[$pf]=array('tt'=>"", 'dw'=>"");
							// ...Tooltip (Menu)
							if(sizeof($la) > 1)
								$tda[$pf]['tt']=trim($la[1]);
							// ...Description (Workflow)
							if(sizeof($la) > 2)
								$tda[$pf]['dw']=trim($la[2]);
						}
					}
				}
			}
//fxDebug($tda,'$tda', 0);

			// Seconds, get menu structure
			$pl=-1;
			$id='';
			$wq='';
			foreach($fe as $line)
			{
				$tline=rtrim(str_replace("\r", '', $line));
				$line=ltrim($tline);
				if(strlen($line))
				{
					$l1=substr($line,0,1);
					$l2=substr($line,0,2);
					if($l1 == '*')											// Workflow question
						$wq=trim(substr($line,1));
					else if(($l1 != '#') && ($l2 != '//') && ($l1 != '>'))	// Menu entry
					{
						$ls=min($pl+1, strlen($tline)-strlen($line));
						$ll=($ls+1)*3-1;
						if(strlen($id) < $ll)
						{
							if(strlen($id))
								$id .= '_';
							if(!strlen($id))	// First entry: Sysadmin
								$id='00';
							else
								$id .= '01';
						}
						else
						{
							$li=1+(int)substr($id,$ll-2,2);
							$id=substr($id,0,$ll-2).substr('00'.$li, -2);
						}
//echo('<pre>[ls='.$ls.']-[id='.$id.']-'.$line.'</pre>');

						$si='M'.$id;
						$pl=$ls;

						// tx=Transaction or menu name | tr=Transaction number (0 for menu) | im=Icon | tt/mk=Mask screenshot number
						$la=explode('|',$line);
						$pf=0;
						if(sizeof($la) > 1)
							$pf=(int)trim($la[1]);
						if($pf == 195)	// Report Rights
						{
							$si=substr($si,0,-2).'99';
							$rr=$si;
						}
						$tx=trim($la[0]);
						$ca[$si]=array('tmid'=>$si, 'lv'=>substr_count($si,'_'), 'tr'=>$pf, 'tx'=>$tx, 'im'=>"", 'tt'=>"", 'dw'=>"", 'wq'=>$wq, 'mk'=>0, 'uf'=>"", 'up'=>"", 'ar'=>"", 'ta'=>0, 'ac'=>0, 'da'=>0, 'at'=>0, 'pa'=>"", 'ic'=>"", 'li'=>"", 'ti'=>0, 'um'=>0, 'uf'=>0, 'up'=>0, 'zs'=>"", 'pb'=>"", 'eb'=>"", 'disp'=>false);

						if(!$pf)
						{
							// Menu entry: tx=Menu name | 0 | im=Icon | tt=Menu tooltip
							// ...Icon
							if(sizeof($la) > 2)
								$ca[$si]['im']=trim($la[2]);
							// ...Tooltip (Menu)
							if(sizeof($la) > 3)
								$ca[$si]['tt']=trim($la[3]);
						}
						else
						{
							// Transaction entry: tx=Transaction name | tr=Transaction number (> 0) | mk=Mask screenshot number
							// ...Mask screenshot nummer
							if(sizeof($la) > 2)
								$ca[$si]['mk']=(int)$la[2];

							// ...Tooltip/Description?
							if(isset($tda[$pf]))
							{
								$ca[$si]['tt']=$tda[$pf]['tt'];
								$ca[$si]['dw']=$tda[$pf]['dw'];
							}

							// Extended Block Transaction?
							$ebp=strpos($tx,'^');
							if($ebp)
							{
								$ca[$si]['pb']=(int)substr($tx,0,$ebp);
								$ca[$si]['eb']=(int)substr($tx,0,$ebp);
								$ca[$si]['tx']=substr($tx,$ebp+1);
							}
						}

						// Special: mym=My Masks
						if($ca[$si]['im'] == 'mym')
						{
							$mg=$si;
						}
						// Special: myr=My Reports
						else if($ca[$si]['im'] == 'myr')
						{
							$rg=$si;
						}
//echo('<pre>[si='.$si.'] {[tx='.$ca[$si]['tx'].'], [tr='.$ca[$si]['tr'].'], [tt='.$ca[$si]['tt'].']}</pre>');

						$wq='';
					}
				}
			}
		}
//echo('$mg='.$mg.', $rg='.$rg.', $rr='.$rr.'<hr />');

		// Sort
		if(fxIsArray($ca))
			ksort($ca,SORT_STRING);
//fxDebug($ca,'$ca');

		// Get additional infos for transactions...
		if(fxIsArray($ca))
		{
			// ...Action code, default action and transaction id
			$sql="SELECT trans_id,aktionscode,defaultaktion,trans_akt_id FROM trans_aktionen ORDER BY trans_id";
			$tmp=db_values($sql,'*');
			if(fxIsArray($tmp))
			{
				$tri=array();
				foreach($tmp as $t)
					$tri[$t['trans_id']]=$t;
//fxDebug($tri,$sql);
				foreach($ca as $id => $mi)
				{
					$mitr=$mi['tr'];
					if(fxIsArray($tri[$mitr]))
					{
						$ca[$id]['ac']=$tri[$mitr]['aktionscode'];
						$ca[$id]['da']=$tri[$mitr]['defaultaktion'];
						$ca[$id]['ti']=$tri[$mitr]['trans_akt_id'];
					}
				}
			}

			// ...Type, parameter, alias transaction and responsible person
			$sql="SELECT transakt_id, trans_art, param, alias_trans, zustaendig FROM trans ORDER BY transakt_id";
			$tmp=db_values($sql,'*');
			if(fxIsArray($tmp))
			{
				$tri=array();
				foreach($tmp as $t)
					$tri[$t['transakt_id']]=$t;
//fxDebug($tri,$sql);
				foreach($ca as $id => $mi)
				{
					$mitr=$mi['tr'];
					if(fxIsArray($tri[$mitr]))
					{
						$ca[$id]['ta']=$tri[$mitr]['trans_art'];
						$ca[$id]['at']=$tri[$mitr]['alias_trans'];
						$ca[$id]['pa']=$tri[$mitr]['param'];
						$ca[$id]['zs']=$tri[$mitr]['zustaendig'];
					}
				}
			}
//fxDebug($ata,'$ata');
		}

		// Save global menu
		if(fxIsArray($ca))
		{
			$txt="<"."?"."php \$ma=array(";
			$cnt=1;
			$soa=sizeof($ca);
			foreach($ca as $mnr => $mda)
			{
				$txt .= fxWriteVar($mnr,$mda);
				if($cnt < $soa)
					$txt .= ",".$GLOBALS['nl_b'];
				$cnt++;
			}
			$txt .= "); ?".">";

			fxSave($gmenu, $txt, true);
		}
	}

	// Load global menu
	$ma=array();
	if(file_exists($gmenu))
	{
		require($gmenu);
	}
//fxDebug($ma,'$ma (new)',0);

	// Always check extended block transactions
	if(fxIsArray($ma))
	{
		foreach($ma as $mnr => $mi)
		{
			// Extended Block Transaction?
			if($mi['tr'] && strlen($mi['eb']) && function_exists('fxsppCheckEBTR') && fxsppCheckEBTR($mi['tr']))
				$ma[$mnr]['eb']="";
		}
	}

	// Check rights
	if((($all !== true) || !$hide) && fxIsArray($ma))
	{
		// Hide hideable transactions?
		$hide_hideable=false;
		if(!$all && substr(strtoupper($GLOBALS['fxpglobals']['settings']['ausbl_tr_anzeig']),0,1) == 'N')
			$hide_hideable=true;

		// Special rights to transactions
		$rights_wm=tr_rechte(83);	// Rights to "Time registration (week/month)"

		if(!isset($GLOBALS['prgclass']))
			getPrgClass();
//echo('prgclass=<b>'.$GLOBALS['prgclass'].'</b><hr />');

		// Check rights on transaction
		foreach($ma as $mnr => $mi)
		{
			$disp=false;
			if($mi['tr'])
			{
//echo('<b>'.$mnr.'</b> -- Check transaction '.$mi['tr'].' = "'.$mi['tx'].'" ('.$mi['ta'].')... ');
				// Sysadmin is omnipotent
				if(!$GLOBALS['fxpglobals']['person'])
					$disp=true;
				// Check transaction
				else
				{
					$trr=tr_rechte($mi['tr']);
//echo('[rights='.$trr.']... ');
					switch($mi['ta'])
					{
						case 40:	// Sysadmin
							if($GLOBALS['prgclass'] >= $mi['ta'])
								$disp=true;
						break;

						case 30:	// Admin1
							if(($all === 2) || (($GLOBALS['fxpglobals']['client'] == 1) && ($all || ($GLOBALS['prgclass'] >= $mi['ta']) || $trr)))
								$disp=true;
						break;

						case 27:	// Admin1-User
							if(($all === 2) || (($GLOBALS['fxpglobals']['client'] == 1) && ($all || ($GLOBALS['prgclass'] >= 30) || $trr)))
								$disp=true;
						break;

						case 20:	// Admin
							if($all || ($GLOBALS['prgclass'] >= $mi['ta']) || $trr)
								$disp=true;
						break;

						case 17:	// Admin-User-Program
							if($all || ($GLOBALS['prgclass'] >= 20) || $trr)
								$disp=true;
						break;

						case 10:	// User-Program
 	 						if($all || (($GLOBALS['prgclass'] >= 10) && ($GLOBALS['prgclass'] < 40) && $trr))
								$disp=true;
						break;

						case 1:		// Standard-Programme
							$disp=true;
						break;
					}
				}

				// Display extended block transactions?
				if($disp && strlen($mi['eb']) && $hide)
					$disp=false;

				// Check, if hideable transactions should be displayed
				if($disp && $hide_hideable && strlen($mi['pa']) && (strpos($mi['pa'],'$ausblendbar') !== false))
					$disp=false;

				// Hide transaction [Time registration (day)], if transaction [Time registration (week/month)] is active?
				if($disp && ($mi['tr'] == 81) && ($rights_wm > 0) && (strtoupper(substr($GLOBALS['fxpglobals']['settings']['ze_tag_ausbl'],0,1)) == 'J'))
					$disp=false;
//echo('$disp='.$disp.'<br />');

				// Display all parents if transaction is displayed
				$ma[$mnr]['disp']=$disp;
				if($disp && (strlen($mnr) > 2))
				{
					$pmnr=substr($mnr,0,-3);
					while(strlen($pmnr) > 1)
					{
						$ma[$pmnr]['disp']=$disp;
						$pmnr=substr($pmnr,0,-3);
					}
				}
			}
		}

		// Delete hidden entries
		if($hide)
		{
			foreach($ma as $mnr => $mi)
			{
				if(!$mi['disp'])
				{
//echo('DELETE: '.$mnr.' - '.$mi['tx'].'<br />');
					unset($ma[$mnr]);
				}
			}
		}
	}

	// Sort
	if(fxIsArray($ma))
		ksort($ma);

//fxDebug($ma,'$ma');
	return $ma;
}

/**
 * Get all alias program functions (transactions) inside a menu array
 *
 * @param various $ma - Mandatory parameter:  Menu array
 *
 * @return Array of all alias program functions
 */
function getAliasTrans($ma)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ata=array();
	if(fxIsArray($ma))
	{
		foreach($ma as $mnr => $mi)
		{
			$mtr=(int)$mi['tr'];
			$atr=(int)$mi['at'];
			if($mtr && $atr)
			{
				if(!isset($ata[$mtr]))
					$ata[$mtr]=array('source'=>$atr, 'alias'=>array());
				else
					$ata[$mtr]['source']=$atr;

				if(!isset($ata[$atr]))
					$ata[$atr]=array('source'=>0, 'alias'=>array($mtr=>true));
				else
					$ata[$atr]['alias'][$mtr]=true;
			}
		}
	}

	return $ata;
}

/**
 * Convert a string into a variable by evaluation
 *
 * @param various $str   - Mandatory parameter:                  String to be converted
 * @param boolean $value - Optional parameter (default = true):  Value
 *
 * @return Converted variable as an array
 */
function str2var($str,$value=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$n=$str;
	$v=$value;
	if(strlen($str))
	{
		$ska=explode('[',$str);
		$skl=sizeof($ska);

		$n=$ska[0];

		if($skl > 1)
		{
			$e='$v=array(';
			$o=1;
			foreach($ska as $sc => $sv)
			{
				if($sc > 0)
				{
					$sv=substr($sv,0,-1);
					if((string)((int)$sv) !== (string)$sv)
						$sv = "'".$sv."'";
					$e .= $sv.'=>';
					if($sc < $skl-1)
					{
						$e .= 'array(';
						$o++;
					}
					else if(is_bool($value))
					{
						if($value)
							$e .= 'true';
						else
							$e .= 'false';
					}
					else if(is_string($value))
						$e .= "'".str_replace("'","´",$value)."'";
					else
						$e .= $value;
				}
			}
			$e .= str_repeat(')',$o).';';
//fxDebug($e, '$e');
			eval($e);
		}
	}

	return array('var'=>$n, 'value'=>$v);
}

/**
 * Write user log
 *
 * @param various $tdc0 - Mandatory parameter:  Start timestamp
 * @param various $tdc1 - Mandatory parameter:  End timestamp
 */
function loginProtocol($tdc0,$tdc1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(($GLOBALS['fxpglobals']['client'] > 0) && ($GLOBALS['fxpglobals']['user'] > 0) && strlen($GLOBALS['fxpglobals']['person']) && strlen($GLOBALS['fxpglobals']['cname']) && strlen($GLOBALS['fxpglobals']['uname']) && strlen($GLOBALS['fxpglobals']['pname']))
	{
		// IP
		$ip=$_SERVER['REMOTE_ADDR'];
		if(!strlen($ip))
			$ip='Anonymous';

		// ID
		$lpid=$GLOBALS['fxpglobals']['client'].'_'.$GLOBALS['fxpglobals']['user'].'_'.$GLOBALS['fxpglobals']['person'].'_'.$ip;
//echo('$lpid='.$lpid.'<br />');

		$lpfiledir=$GLOBALS['logpath'];
		$lpfilename=$lpfiledir.'~'.substr($GLOBALS['date'],0,6).'.log';
//echo('$lpfiledir='.$lpfiledir.' - $lpfilename='.$lpfilename.'<br />');

		// Create directory if non existent
		$lpdirex=true;
		if(!is_dir($lpfiledir))
		{
			if(!@mkdir($lpfiledir, 0777))
				$lpdirex=false;
		}

		if($lpdirex)
		{
			// Read log file content
			$lpcontent='';
			if(file_exists($lpfilename))
				$lpcontent=fxLoad($lpfilename);
//echo('$lpcontent=<pre>'.$lpcontent.'</pre>');

			// Username (Sysadmin user)
			$luname=$GLOBALS['fxpglobals']['uname'];
			if($GLOBALS['fxpglobals']['suser'])
				$luname='^'.$luname;

			// Log file empty, create new entry
			if(!strlen($lpcontent))
			{
				$lpcontent=$lpid.'|'.$tdc0.'|'.$tdc1.'|'.$GLOBALS['fxpglobals']['cname'].'|'.$luname.'|'.$GLOBALS['fxpglobals']['pname'].'|'.$GLOBALS['fxpglobals']['tr'].'_'.$tdc0.'_'.$tdc1;
				fxSave($lpfilename, $lpcontent, true);
			}
			else
			{
				$lpcreatenew=true;

				// Check for last previous user entry
				$lpidspos=strrpos($lpcontent,$lpid.'|');
				// ...Found
				if($lpidspos !== false)
				{
					// Extract entry line
					$lpidepos=strpos($lpcontent,$GLOBALS['nl_b'],$lpidspos);
					if($lpidepos === false)
						$lpidepos=strlen($lpcontent);
					$lpline=substr($lpcontent,$lpidspos,$lpidepos-$lpidspos);
					$lpdata=explode('|',$lpline);
//fxDebug($lpdata,$lpline);

					// Check for time diff
					$lptimediff=floor($tdc0-$lpdata[2]);
					// ...less than 1/2 hour = 30 min = 1800 sec - continue old session
					if($lptimediff < 1800)
					{
						$lpcreatenew=false;

						// Set new session end timestamp
						$lpdata[2]=$tdc1;

						// Get last user transaction
						$lptrpos=strrpos($lpdata[6],',');
						if($lptrpos === false)
							$lptrpos=0;
						else
							$lptrpos++;
						$lptrentry=substr($lpdata[6],$lptrpos);
						$lptrdata=explode('_',$lptrentry);

						// Same transaction, adjust end timestamp
						if($lptrdata[0] == $GLOBALS['fxpglobals']['tr'])
							$lpdata[6] = substr($lpdata[6],0,$lptrpos).$lptrdata[0].'_'.$lptrdata[1].'_'.$tdc1;
						else
							$lpdata[6] .= ','.$GLOBALS['fxpglobals']['tr'].'_'.$tdc0.'_'.$tdc1;

						// Adjust entry line
						$lpline=$lpdata[0].'|'.$lpdata[1].'|'.$lpdata[2].'|'.$lpdata[3].'|'.$lpdata[4].'|'.$lpdata[5].'|'.$lpdata[6];
						$lpcontent=substr($lpcontent,0,$lpidspos).$lpline.substr($lpcontent,$lpidepos);
					}
				}

				// Append new user entry
				if($lpcreatenew)
				{
					$lpcontent=$lpid.'|'.$tdc0.'|'.$tdc1.'|'.$GLOBALS['fxpglobals']['cname'].'|'.$luname.'|'.$GLOBALS['fxpglobals']['pname'].'|'.$GLOBALS['fxpglobals']['tr'].'_'.$tdc0.'_'.$tdc1;
					fxSave($lpfilename, $lpcontent, false);
				}
				// Adjust old user entry
				else
					fxSave($lpfilename, $lpcontent, true);
			}
		}
	}
}

/**
 * Get database encoded text identifiers
 *
 * @param various $pw     - Mandatory parameter:                  Source string (only used if direction is "fromdb")
 * @param boolean $frompw - Optional parameter (default = true):  Direction: Get identifiers from db string (true) or create new identifier (false)
 *
 * @return Encoded text identifiers
 */
function db_pw_micro_db($pw, $frompw=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($frompw)
	{
		$mic1=substr($pw,0,1);	// ~
		$mic3=substr($pw,2,1);	// #
		return($mic1.$mic3);
	}
	else
		return '~'.chr(181).'#';
}

/**
 * Encode or decode database text
 *
 * @param various $pw - Mandatory parameter:  Database text
 *
 * @return Encoded or decoded database text (depends on determined edentifiers)
 */
function db_pw_codec_db($pw)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

//echo('<B>db_pw_codec_db()</B><br />'.$GLOBALS['nl']);
	$mic=db_pw_micro_db($pw);
//echo('Altes PW: <B>'.$pw.'</B> -- $mic: ['.$mic.']<br />');
	if($mic === '~#')
	{
		// Passwort decodieren
		$anzahl		= ord(substr($pw, 4, 1))-80;
		$richtung	= ord(substr($pw, 5, 1));
		$add		= ord(substr($pw, -3, 1))-70;
		$maxz=strlen($pw)-4;
		$npw_m='';
		for($zz=7; $zz<$maxz; $zz++)
			$npw_m .= substr($pw, $zz, 1);
		if($richtung%2 == 1)	// Ungerade, d.h. String drehen
			$npw_m=strrev($npw_m);
		$npw='';
		$zz=0;
		while($zz<strlen($npw_m))
		{
			$bank=ord(substr($npw_m, $zz, 1)) - 65;
			$zz++;
			$wert=ord(substr($npw_m, $zz, 1)) - 65;
			$zz++;
			$zok=$bank*25 + $wert - $add;
			$npw .= chr($zok);
			for($az=$anzahl; $az>1; --$az)
				$zz++;
		}
	}
	else
	{
		// Passwort codieren
		srand((double) microtime() * 1000000); // Zufallsgenerator initialisieren
		$len=strlen($pw);
		if($len > 128)
			$anzahl=0;
		else if(isset($anzahl) && ($anzahl > 64))
			$anzahl=rand(0, 1);
		else if(isset($anzahl) && ($anzahl > 32))
			$anzahl=rand(0, 2);
		else
			$anzahl=rand(0, 3);
		$richtung=rand(0, 25) + 65;
		$add=rand(0, 5) + 2;

		// Linker Teil
		$npw_l=chr(rand(0, 25) + 65).chr(80+$anzahl).chr($richtung).chr(rand(0, 25) + 65);

		// Mittlerer Teil
		$npw_m='';
		for($zz=0; $zz<strlen($pw); $zz++)
		{
			$zok=ord(substr($pw, $zz, 1)) + $add;
			$bank=(int)($zok/25);
			$wert=$zok%25;
			$npw_m .= chr($bank + 65);
			$npw_m .= chr($wert + 65);
			for($az=$anzahl; $az>1; --$az)
				$npw_m .= chr(rand(0, 25) + 65);
		}
		if($richtung%2 == 1)	// Ungerade, d.h. String drehen
			$npw_m=strrev($npw_m);

		// Rechter Teil
		$npw_r=chr(rand(0, 25) + 65).chr(70+$add).chr(rand(0, 25) + 65).chr(rand(0, 25) + 65);
		$npw=db_pw_micro_db('', false).$npw_l.$npw_m.$npw_r;
	}
//echo('Neues PW: <B>'.$npw.'</B><br />');
	return $npw;
}

/**
 * Encode a database text until it is not longer than a maximum length
 *
 * @param various $code   - Mandatory parameter:                Text to be encoded
 * @param integer $maxlen - Optional parameter (default = 50):  Maximum length of encoded text
 *
 * @return Encoded database text
 */
function db_max_pw_codec_db($code, $maxlen=50)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ec=0;
	$codec='';
	while(($ec < 40) && (($codec === '') || (strlen($codec) > $maxlen)))
	{
		$codec=db_pw_codec_db($code);
		$ec++;
	}

	return $codec;
}

/**
 * Return default code id, or first and third character of a string (to check if it's an encoded string)
 *
 * @param integer $get - Optional parameter (default = 0):   0=Get first and third character of the string, 2=Get 2 character default id, else=Get 3 character default id
 * @param string $pw   - Optional parameter (default = ''):  String to be checked
 *
 * @return string -> Default code id, or first and third character of string
 */
function fxf_paramCodeId($get=0, $str='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$get=(int)$get;

	if(!$get)			// Get first and third charcater from string
	{
		$id='';
		if(strlen($str))
			$id .= substr($str,0,1);	// ~
		if(strlen($str) > 2)
			$id .= substr($str,2,1);	// #
	}
	else if($get == 2)	// Get 2 character default id
		$id='~#';
	else				// Get 3 character default id
		$id='~!#';

	return($id);
}

/**
 * Encode or decode a string
 *
 * @param string $str  - Mandatory parameter:               String to be en- or decoded
 * @param integer $enc - Optional parameter (default = 0):  0=Toggle between encode and decode, <0=Decode, >0=Encode
 *
 * @return En- or decoded string
 */
function fxf_paramCodec($str, $enc=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$enc=(int)$enc;
	$rst=$str;

	// Get code id's
	$ids=fxf_paramCodeId(0,$str);
	$id2=fxf_paramCodeId(2);
	$id3=fxf_paramCodeId(3);

	// Get keys
	$k32='';
	$k64='';
	if(fxIsArray($GLOBALS['fxpglobals']['dbparam']) && isset($GLOBALS['fxpglobals']['dbparam']['oskey32']) && isset($GLOBALS['fxpglobals']['dbparam']['oskey64']))
	{
		$k32=base64_decode($GLOBALS['fxpglobals']['dbparam']['oskey32']);
		$k64=base64_decode($GLOBALS['fxpglobals']['dbparam']['oskey64']);
	}
	$m='aes-256-cbc';
	$ivl=openssl_cipher_iv_length($m);

	// String is encoded
	if($ids === $id2)
	{
		// ...Decode?
		if($enc <= 0)
		{
			$x=base64_decode(substr($str,strlen($id3)));
			$iv=substr($x,0,$ivl);
			$e64=substr($x,$ivl,64);
			$e32=substr($x,$ivl+64);
			$rst=openssl_decrypt($e32,$m,$k32,OPENSSL_RAW_DATA,$iv);
			$h64=hash_hmac('sha3-512',$e32,$k64,true);
			if(!hash_equals($e64,$h64))
				$rst='[invalid]';
		}
	}
	// String is not encoded
	else
	{
		// ...Encode?
		if($enc >= 0)
		{
			$iv=openssl_random_pseudo_bytes($ivl);
			$r32=openssl_encrypt($str,$m,$k32,OPENSSL_RAW_DATA,$iv);
			$r64=hash_hmac('sha3-512',$r32,$k64,true);
			$rst=$id3.base64_encode($iv.$r64.$r32);
		}
	}

	return $rst;
}

/**
 * Get check key that determines if user has changed browser, cleared cache or reloaded a page
 *
 * @return Browser check key
 */
function getSSL()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Session storage id
	$c=$GLOBALS['fxpglobals']['sesstoid'];
	// Login timestamp
	$l=$GLOBALS['lts'];
	// Date
	$d=date('Ymd');
	// IP
	$i=$_SERVER['REMOTE_ADDR'];
	if(!strlen($i))
		$i='(anonymous)';
	// Browser
	$b=$_SERVER['HTTP_USER_AGENT'];
	if(!strlen($b))
		$b='(undefined)';


	$s=$c.'|'.$l.'|'.$d.'|'.$i.'|'.$b;
	$r=md5($s);
//echo('$c='.$c.', $l='.$l.', $d='.$d.', $i='.$i.', $b='.$b.'<hr />'.$s.'<hr />'.$r.'<hr />');

	return $r;
}

/**
 * Create a random hexadecimal password string from characters 0-9 and a-f
 *
 * @param integer $len - Optional parameter (default = 8):  Password string length
 *
 * @return Created random password string
 */
function hexPassword($len=8)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$len=(int)$len;
	$pw='';
	if($len >= 1)
	{
		$hexcodes='0123456789abcdef';
		for($l=1; $l<=$len; $l++)
		{
			$rand_hex=mt_rand(0, 15);
			$pw .= substr($hexcodes, $rand_hex, 1);
		}
	}
	return $pw;
}

/**
 * Encode/Check password
 *
 * @param string  $pw  - Mandatory parameter:                Password
 * @param boolean $pc  - Optional parameter (default = ''):  Check password: If empty the password will be encoded, else it will be checked against this value
 * @param integer $cid - Optional parameter (default = 0):   Client ID (if 0 the current session value is used)
 * @param integer $uid - Optional parameter (default = 0):   User ID (if 0 the current session value is used)
 *
 * @return Encoded password (for Encode mode) or True if password is valid else False (for Check mode)
 */
function fxf_ecPasswd($pw, $pc='', $cid=0, $uid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Client/User ID
	$pc=trim($pc);
	$cid=(int)$cid;
	if($cid <= 0)
		$cid=$GLOBALS['fxpglobals']['client'];
	$uid=(int)$uid;
	if($uid <= 0)
		$uid=$GLOBALS['fxpglobals']['user'];

	$pws='pw_c'.$cid.'_u'.$uid.'|'.$pw.'|'.$GLOBALS['fxpglobals']['dbparam']['oskey32'];
	$pw5=md5($pws);

	// Encode password?
	if(!strlen($pc))
		$pwr=hexPassword(5).$pw5.hexPassword(3);
	// Check password?
	else
	{
		$pc5=substr($pc,5,-3);
		$pwr=false;
		if($pw5 === $pc5)
			$pwr=true;
	}

	return $pwr;
}

/**
 * Write basic log into file $GLOBALS['fxpbfilename']
 *
 * @param various $lline - Mandatory parameter:  Log line to be written
 */
function bLog($lline)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$wfilename='';
	$ts=date('Y-m-d H:i:s',time());
	$trace=debug_backtrace();
	$tline=$trace[0]['line'];
	$tfile=$trace[0]['file'];
	$prefix=$ts."\t{".substr('00000'.(int)$_GET['lcnt'],-5).'-'.substr('000'.(int)$GLOBALS['fxpglobals']['tr'],-3)."}:\t".$tfile." [".substr('00000'.$tline,-5)."]\t";

	// Determine new line
	$lnl="\r\n";
	if(isset($_ENV) && (isset($_ENV['PATH']) && (substr($_ENV['PATH'], 0, 1) == '/') || (isset($_ENV['Path']) && substr($_ENV['Path'], 0, 1) == '/') || (isset($_ENV['path']) && substr($_ENV['path'], 0, 1) == '/')))
		$lnl="\n";

	// Start log?
	if($lline === '*START*')
	{
		$wfilename=$GLOBALS['logpath'].'_basics.log';
		$GLOBALS['fxpbfilename']=$wfilename;
		$lline='';
		if(file_exists($wfilename))
			$lline=$lnl.$lnl;
		$lline .= '____________________________________________________________________________________________________________________________________________________________________________________________________________________________'.$lnl;
		$lline .= $prefix.'LOGSTART'.$lnl;
		$lline .= '°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°'.$lnl;
	}
	else if(isset($GLOBALS['fxpbfilename']) && strlen($GLOBALS['fxpbfilename']))
	{
		$wfilename=$GLOBALS['fxpbfilename'];
		$lline=$prefix.$lline.$lnl;
	}

	if(strlen($wfilename))
		fxSave($wfilename, $lline, false);
}

/**
 * Get help text from help file
 *
 * @param various $filename   - Mandatory parameter:                  Filename of the help file
 * @param boolean $withborder - Optional parameter (default = true):  Create help text with surrounding borders (true) or without (false)
 *
 * @return Adjusted help text
 */
function getHelpText($filename,$withborder=true,$width='width:664px;')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$desc=meldung(31);	// empty
	$html=fxLoad($filename);
	if(strlen($html))
	{
		// Extract text
		$bsp=stripos($html,'<body');
		if($bsp !== false)
		{
			$etp=strpos($html,'>',$bsp+5);
			$html=trim(substr($html,$etp+1));

			$bep=stripos($html,'</body',$bsp);
			if($bep !== false)
				$html=trim(substr($html,0,$bep));
		}

		// Div replacement
		$dsp=stripos($html,'<div');
		if($dsp !== false)
		{
			$etp=strpos($html,'>',$dsp+4);
			$cls='';
			if($withborder)
				$cls=' class="rtable"';
			$html='<div'.$cls.' style="background:#ffffff;'.$width.'text-align:left;padding:12px;white-space:normal;">'.trim(substr($html,$etp+1));
		}

		// Page break replacement
		$psp=stripos($html,'<br /><br clear');
		if($psp !== false)
		{
			$etp=strpos($html,'>',$psp+13);
			$html=trim(substr($html,0,$psp)).trim(substr($html,$etp+1));
		}

		// Path replacements
		$dp=str_replace("\\", '/', trim(dirname($filename)));
		if(substr($dp,-1) != '/')
			$dp .= '/';
		$ra=array();
		$sa=array('DOC'=>$dp, 'GFX'=>$GLOBALS['gfxpath'], 'SCR'=>$dp.'SCR/');
		foreach($sa as $st => $sp)
		{
			$ra['"'.$st.'/']='"'.$sp;
			$ra['"./'.$st.'/']='"'.$sp;
			$ra['"../'.$st.'/']='"'.$sp;
		}
//fxDebug($ra,'$ra', 0);
		$desc=strtr($html,$ra).'<br />';
	}

	return $desc;
}

/**
 * Get current computer name
 *
 * @return Current computer name
 */
function getComputerName($fxp=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Get computer name
	$cn=$_SERVER['COMPUTERNAME'];
	if(!strlen($cn))
		$cn=$_SERVER['SERVER_NAME'];
	if(!strlen($cn))
		$cn=$_SERVER['HTTP_HOST'];

	if(strlen($cn))
		$cn=strtoupper($cn);

	return $cn;
}

/**
 * Get formatted link
 *
 * @param various $href      - Mandatory parameter:                      URL link
 * @param string  $download  - Optional parameter (default = ''):        Add download filename to link
 * @param string  $text      - Optional parameter (default = ''):        Optional text to be displayed. If empty then the anchor end tag will also not be displayed
 * @param string  $style     - Optional parameter (default = ''):        Add style to link
 * @param string  $target    - Optional parameter (default = '_blank'):  Link target
 *
 * @return Formatted link
 */
function fxpALink($href, $download='', $text='', $style='', $target='_blank')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$l='<a href="'.$href;
	$qp=strpos($href,'?');
	if($qp === false)
		$l .= '?';
	else
		$l .= '&';
	$l .= substr(fxSetSession(),1).'"';
	if(strlen($download))
		$l .= ' download="'.$download.'"';
	if(strlen($style))
		$l .= ' style="'.$style.'"';
	$l .= ' target="'.$target.'">';
	if(strlen($text))
		$l .= $text.'</a>';

	return $l;
}

/**
 * Get all valid designs for a client
 *
 * @param integer $c - Optional parameter (default = 0):  Client id (if 0 use current session value)
 * @param integer $u - Optional parameter (default = 0):  User id (if 0 use current session value)
 *
 * @return Array of all valid designs (fx: fx-project default designs, client: extra client designs)
 */
function fxpValidDesigns($c=0,$u=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$c=(int)$c;
	if(!$c && isset($GLOBALS['fxpglobals']['client']))
		$c=(int)$GLOBALS['fxpglobals']['client'];
	$u=(int)$u;
	if(!$u && isset($GLOBALS['fxpglobals']['user']))
		$u=(int)$GLOBALS['fxpglobals']['user'];
	$fxd=true;
	$cld=false;

	$db=checkDB();
	if($db && ($c > 0))
	{
		// Check if user has design program (40) assigned
		if($u > 0)
		{
			$sql="SELECT rechte_trans_id FROM rechte_trans WHERE trans_id=40 AND mandanten_id=".$c." AND benutzergr_id IN (SELECT benutzergr_id FROM benutzergruppenzo WHERE benutzer_id=".$u." AND mandanten_id=".$c.")";
			$val=(int)db_value($sql);
//fxDebug($val,$sql);
			if($val)
				$cld=true;
		}
	}

	return array('fx'=>$fxd, 'client'=>$cld);
}

/**
 * Get all CSS colors for font (cstyle) and background (bstyle) colors in an array where the key is the CSS name and the value is the color, e.g. $GLOBALS['bstyle']['blue']='006b9f'
 *
 * @param boolean $allwhite - Optional parameter (default = false):  If true all background colors will be set to white (= ffffff) - useful for documentation
 */
function fxpGetCSSColors($allwhite=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Load style to get color info
	$cssnr=$GLOBALS['cssfile'];
	$style_array=@file($cssnr);
//fxDebug($style_array,$cssnr, 0);

	$GLOBALS['cstyle']=array();
	$GLOBALS['bstyle']=array();
	if(fxIsArray($style_array))
	{
		$ica=array();
		foreach($style_array as $l)
		{
			$l=strtolower(trim($l));

			// New defintion line?
			if((substr($l,0,1) == '#') || (substr($l,0,1) == '.'))
			{
				$ica=array();
				$bp=strpos($l,'{');
				if($bp !== false)
				{
					$a=explode(',', trim(substr($l,0,$bp)));
					$l=trim(substr($l,$bp+1));
//fxDebug($a,'$a: $l='.$l, 0);
					foreach($a as $ic)
					{
						$ic=trim($ic);
						$sp=strpos($ic,' ');
						if($sp)
							$ic=trim(substr($ic,0,$sp));
						$cp=strpos($ic,':');
						if(($cp === false) && ((substr($ic,0,1) == '#') || (substr($ic,0,1) == '.')))
							$ica[substr($ic,1)]=true;
					}
				}
//fxDebug($ica,'$ica: $l='.$l, 0);
			}

			// New value?
			if(sizeof($ica))
			{
				$cf='';
				$sp=0;
				while(true)
				{
					// Background?
					$fm='';
					$cp=strpos($l,'background',$sp);
					if($cp !== false)
					{
						$fm='b';
						$sp=$cp+10;
					}
					// Color?
					else
					{
						$cp=strpos($l,'color',$sp);
						if($cp !== false)
						{
							$fm='c';
							$sp=$cp+5;
						}
					}
					if(!strlen($fm))
						break;
					$cp=strpos($l,':',$sp);
					if($cp === false)
						break;
					$sp=$cp+1;
					$cp=strpos($l,';',$sp);
					if($cp === false)
						break;
					$cs=substr($l,$sp,$cp-$sp);
					$sp=$cp+1;
					$hp=strpos($cs,'#');
					if($hp !== false)
					{
						$bg=trim(substr($cs,$hp+1,6));
						if($allwhite)
							$bg='ffffff';
						else
						{
							$cp=strpos($bg,',');
							if($cp)
								$bg=trim(substr($bg,0,$cp));
							if(strlen($bg) == 3)
								$bg=substr($bg,0,1).substr($bg,0,1).substr($bg,1,1).substr($bg,1,1).substr($bg,2,1).substr($bg,2,1);
						}
						foreach($ica as $v => $d)
						{
							if(!isset($GLOBALS[$fm.'style'][$v]))
								$GLOBALS[$fm.'style'][$v]=$bg;
						}
					}
				}
			}
		}

		if($allwhite)
			$GLOBALS['bstyle']['tbc']='f9f9f9';
	}
//fxDebug($GLOBALS['cstyle'],'cstyle', 0);
//fxDebug($GLOBALS['bstyle'],'bstyle', 0);
}

/**
 * Taskbar: Get and return all possible designs for a user
 *
 * @param boolean $ret_array - Optional parameter (default = false):  Return designs as array (true) or as text (false)
 * @param integer $c         - Optional parameter (default = 0):      Client id (if 0 use current session value)
 * @param integer $u         - Optional parameter (default = 0):      User id (if 0 use current session value)
 *
 * @return All possible designs for a user
 */
function fxpGetDesigns($ret_array=false,$c=0,$u=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$c=(int)$c;
	if(!$c && isset($GLOBALS['fxpglobals']['client']))
		$c=(int)$GLOBALS['fxpglobals']['client'];
	$u=(int)$u;
	if(!$u && isset($GLOBALS['fxpglobals']['user']))
		$u=(int)$GLOBALS['fxpglobals']['user'];

	$vda=fxpValidDesigns($c,$u);

	$fxdsa=array();

	// 1. Step: Get our designs
	if($vda['fx'])
	{
		$cssda=fxScanDir($GLOBALS['csspath'],false,true,'/^.+\.css$/i');
//fxDebug($cssda,'$cssda', 0);
		if($cssda['file_entries'] > 0)
		{
			foreach($cssda['files'] as $dcssfile => $dcsspath)
			{
				$cssprefix=substr($dcssfile,0,2);
				if(ctype_digit($cssprefix))
				{
					$cssname='';
					$cssh=trim(file_get_contents($dcsspath,false,NULL,0,256));
					if(substr($cssh,0,2) == '/'.'*')
					{
						$cssecp=strpos($cssh,'*',2);
						if($cssecp)
							$cssname=trim(substr($cssh,2,$cssecp-2));
					}
					if(!strlen($cssname))
						$cssname=ucfirst(substr($dcssfile,2,-4));
					$fxdsa['0'.substr($dcssfile,0,-4)]=$cssname;
				}
			}
		}
	}
	if(!fxIsArray($fxdsa))
		$fxdsa=array('000default'=>"Default");

	// 2. Step: Get client designs
	if(($c > 0) && $vda['client'])
	{
		$cssda=fxScanDir($GLOBALS['datpath'].$c.'/',false,true,'/^.+\.css$/i');
//fxDebug($cssda,'$cssda', 0);
		if($cssda['file_entries'] > 0)
		{
			foreach($cssda['files'] as $dcssfile => $dcsspath)
			{
				$cssname='';
				$cssh=trim(file_get_contents($dcsspath,false,NULL,0,256));
				if(substr($cssh,0,2) == '/'.'*')
				{
					$cssecp=strpos($cssh,'*',2);
					if($cssecp)
						$cssname=trim(substr($cssh,2,$cssecp-2));
				}
				if(!strlen($cssname))
					$cssname=ucfirst(substr($dcssfile,2,-4));
				$fxdsa[$c.'C'.substr($dcssfile,0,-4)]=$cssname;
			}
		}
	}
//fxDebug($fxdsa,'$fxdsa', 0);

	// Create design table entries
	ksort($fxdsa);
	if($ret_array)
		return $fxdsa;

	$dhtr_set=false;
	$fxdst  = '<table border="0" cellpadding="0" cellspacing="2">'.$GLOBALS['nl'];
	$fxdst .= '<tr><td colspan="2" align="center"><div style="border-bottom:1px solid #cccccc;margin-top:8px;margin-bottom:4px;"><b>UID</b> <i class="s2" style="color:#666666;">(User Interface Design)</i></div></td></tr>'.$GLOBALS['nl'];
	foreach($fxdsa as $fxdsf => $fxdsn)
	{
		$dhtr='';
		$fxdsi='fxtaskdps_'.$fxdsf;
		$dt=(int)substr($fxdsf,0,1);
		if(!$dt)	// Our designs
		{
			$cf=$GLOBALS['csspath'];
			$df=substr($fxdsf,1);
		}
		else		// Client designs
		{
			$cp=strpos($fxdsf,'C',1);
			$cf=$GLOBALS['datpath'].substr($fxdsf,0,$cp).'/';
			$df=substr($fxdsf,$cp+1);
			if(!$dhtr_set)
			{
				$dhtr_set=true;
				$dhtr='<tr><td colspan="2" align="center"><div style="border-bottom:1px solid #cccccc;color:#888888;margin-top:8px;margin-bottom:4px;"><b>'.$dt.'</b></div></td></tr>'.$GLOBALS['nl'];
			}
		}
		$ic=$cf.$df.'.png';
		if(file_exists($ic))
			$img='<img src="'.$ic.'?'.$GLOBALS['datetime'].'" id="'.$fxdsi.'_img" style="border:1px solid #000000;box-shadow:2px 2px 2px #888888;cursor:pointer;" />';
		else
			$img='';
		$fxdst .= $dhtr.'<tr id="'.$fxdsi.'_tr" style="cursor:pointer;"><td id="'.$fxdsi.'_td" width="30" style="cursor:pointer;">'.$img.'</td><td>&nbsp;<span id="'.$fxdsi.'_span" style="cursor:pointer;" class="black">'.$fxdsn.'</span></td></tr>'.$GLOBALS['nl'];
	}

	if(!$vda['fx'] || !$vda['client'])
	{
		$l=(int)$GLOBALS['fxpglobals']['lang'];
		if(!$vda['fx'])
		{
			if($l == 1)
				$msg='Zusätzliche Designs wurden noch nicht freigeschaltet';
			else
				$msg='Additional designs have not been unlocked yet';
		}
		else if($l == 1)
			$msg='Mandantendesigns wurden noch nicht zugeordnet';
		else
			$msg='Client designs have not been assigned yet';
		$fxdst .= '<tr><td colspan="2" align="center"><i class="lightgrey s1"><br />('.$msg.'!)</i></td></tr>'.$GLOBALS['nl'];
	}

	$fxdst .= '</table>'.$GLOBALS['nl'];

	return $fxdst;
}

/**
 * Get the design path with filename of a specific design
 *
 * @param string  $d - Optional parameter (default = ''):  Name of the design
 * @param integer $c - Optional parameter (default = 0):   Client id (if 0 use current session value)
 * @param integer $u - Optional parameter (default = 0):   User id (if 0 use current session value)
 *
 * @return Design path with filename
 */
function fxpDesignPath($d='',$c=0,$u=0,$re=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($d))
		$d=$GLOBALS['fxpglobals']['design'];
	$d=trim($d);
	if(!strlen($d))
	{
		if($re)
			return '';
		$d='000default';
	}

	$c=(int)$c;
	if(!$c && isset($GLOBALS['fxpglobals']['client']))
		$c=(int)$GLOBALS['fxpglobals']['client'];
	$u=(int)$u;
	if(!$u && isset($GLOBALS['fxpglobals']['user']))
		$u=(int)$GLOBALS['fxpglobals']['user'];

	$vda=fxpValidDesigns($c,$u);
	if(!$vda['fx'] && !$vda['client'])
		return '';

	$dt=(int)substr($d,0,1);
	if(!$dt)	// Our designs
	{
		$df=substr($d,1);
		if(!$vda['fx'])
			return '';

		$cp=$GLOBALS['csspath'].$df.'.css';
	}
	else		// Client designs
	{
		$dp=strpos($d,'C',1);
		$cf=substr($d,0,$dp);
		$df=substr($d,$dp+1);
		if(!$vda['client'])
			return '';

		$cp=$GLOBALS['datpath'].$cf.'/'.$df.'.css';
	}
//echo('$cp='.$cp.'<hr />');

   	if(file_exists($cp))
   		return $cp;

	return '';
}

/**
 * Load and return the CSS design text
 *
 * @param boolean $convert - Optional parameter (default = true):   Convert CSS design to the temporary format (for the relevant program function)
 * @param boolean $dp      - Optional parameter (default = false):  Design path with filename (if false the current one is used)
 * @param string  $css     - Optional parameter (default = ''):     CSS design (is empty the current design is loaded)
 *
 * @return CSS design text
 */
function fxpCSSDesign($convert=true, $dp=false, $css='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($css))
	{
		if($dp === false)
			$dp=fxpDesignPath();
//echo('$dp='.$dp.'<hr />');

		$css='';
		if(strlen($dp))
			$css=fxLoad($dp);
	}

	if(strlen($css) && $convert)
	{
		$dpa=array(
			'body'=>'dp_hbody',
			'#fx'=>'#dp_ifx', '#pj'=>'#dp_ipj', '#sel'=>'#dp_isel', '#fav'=>'#dp_ifav', '#search'=>'#dp_isearch',
			'.ti'=>'.dp_cti', '.tt'=>'.dp_ctt', '.tm'=>'.dp_ctm', '.menu'=>'.dp_cmenu', '.mask'=>'.dp_cmask', '.fm'=>'.dp_cfm', '.focus'=>'.dp_cfocus', '.dfxf'=>'.dp_cdfxf', '.fxf'=>'.dp_cfxf', '.sel'=>'.dp_csel', '.msel'=>'.dp_cmsel', '.btn'=>'.dp_cbtn', '.act'=>'.dp_cact'
		);
		$css=strtr($css,$dpa);
	}
//echo('<pre>'.$css.'</pre>');

	return $css;
}

/**
 * Create a new client design as an array
 *
 * @param various $css  - Mandatory parameter:               CSS design to be used
 * @param integer $ms   - Optional parameter (default = 0):  Number of menu colors to be used (if 0 all are used)
 * @param integer $lang - Optional parameter (default = 0):  Language of the texts to be displayed in the relevant program function (if 0 the current session value is used)
 *
 * @return Client design as array
 */
function fxpClientDesign($css,$ms=0,$lang=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ms=(int)$ms;
	if(!$ms)
	{
		$ma=loadMenu(true);
		foreach($ma as $id => $sa)
		{
			if(!$sa['lv'] && ($sa['tx'] != 'Sysadmin'))
				$ms++;
		}
	}

	$dsa=array(
		'cfxbody'		=>	array(	'name'=>"Background",
									'desc'=>"Global continuous background (will be overlayed by the navigation toolbar, the menu bar and also by the work area)",
									'background*'=>"2|#eeeeee,#666666",
									'color'=>"#000000"),
		'ifxtoolc'		=>	array(	'name'=>"Navigation Toolbar",
									'desc'=>"Topmost toolbar area for executing the most important program functions like filter, settings, help, etc.",
									'background*'=>"#666666,#404040,#404040",
									'border-bottom'=>"#000000"),
		'cticontxt'		=>	array(	'name'=>"Navigation Icons",
									'desc'=>"Tools for calling the most important program functions like filter, settings, help, etc.",
									'color'=>"#ffffff"),
		'ifxsrcinput'	=>	array(	'name'=>"Navigation Search",
									'desc'=>"Search field for listing and finding program functions",
									'background'=>"#ffffff"),
		'cttitle'		=>	array(	'name'=>"Breadcrumb Navigation",
									'desc'=>"Complete path of the actual executed program function",
									'color'=>"#999999"),
		'cactreg'		=>	array(	'name'=>"Action Standard",
									'desc'=>"Action button that is clickable",
									'background'=>"#969696",
									'color'=>"#eeeeee"),
		'cactrega'		=>	array(	'name'=>"Action Active",
									'desc'=>"Action button of the actual active action mode",
									'background'=>"#404040",
									'color'=>"#ffffff"),
		'cactregi'		=>	array(	'name'=>"Action Inactive",
									'desc'=>"Action button that is not clickable",
									'color'=>"#777777"),
		'ifxheaderc'	=>	array(	'name'=>"Work Area Header",
									'desc'=>"Header area of the work area with buttons",
									'background*'=>"#ffffff,#888888,#888888,#777777",
									'color'=>"#ffffff"),
		'ifxwork'		=>	array(	'name'=>"Work Area Main",
									'desc'=>"Main area of the work area in which all masks are displayed",
									'background*'=>"#fafafa",
									'color'=>"#000000",
									'border-top'=>"#444444"),
		'ifxfooterc'	=>	array(	'name'=>"Work Area Foot",
									'desc'=>"Footer of the work area with additional infos and copyright notice",
									'background*'=>"#777777,#666666,#666666,#444444",
									'border-top'=>"#aaaaaa",
									'border-bottom'=>"#000000"),
		'cmaskh'		=>	array(	'name'=>"Mask Header",
									'desc'=>"Header area of the mask with title",
									'background*'=>"#eeeeee,#dddddd,#dddddd,#cccccc",
									'color'=>"#111111",
									'border-bottom'=>"#bbbbbb"),
		'cmaskc'		=>	array(	'name'=>"Mask Content",
									'desc'=>"Content area of the mask",
									'background'=>"#f0f0f0",
									'color'=>"#000000",
									'border-left'=>"#ffffff",
									'border-top'=>"#ffffff",
									'border-right'=>"#bbbbbb",
									'border-bottom'=>"#bbbbbb"),
		'cfxfbs'		=>	array(	'name'=>"Button",
									'desc'=>"Button in the header work area for saving or inside masks for selection etc.",
									'background'=>"#a0a0a0",
									'color'=>"#ffffff",
									'color:hover'=>"#000000",
									'border-color'=>"#ffffff",
									'border-color:hover'=>"#888888"),
		'ifxtask'		=>	array(	'name'=>"Taskbar",
									'desc'=>"Line at the bottom for displaying user infos, languages etc.",
									'background*'=>"#444444,#000000,#000000",
									'color'=>"#888888",
									'border-top'=>"#666666"),
		'cmenu1-i'		=>	array(	'name'=>"Standard menu background",
									'desc'=>"Standard background color of the 1st menu level",
									'background'=>"#0d0d0d")
	);

	$da=array('#006b9f', '#006b9f', '#009f6b', '#d4c53c', '#b33d8c');	// Default menu colors
	$cp=false;
	if(strlen($css))
		$cp=stripos($css,'cmenu-c0 {');
	for($c=0; $c<=$ms; $c++)
	{
		$bg='';
		if(($cp === false) && isset($da[$c]))
			$bg=$da[$c];
		$dsa['cmenu-c'.$c]=array('name'=>"Category section color ".$c, 'desc'=>"Background color for category ".$c." menus", 'background'=>$bg);
	}

	$lang=(int)$lang;
	if(!$lang)
		$lang=$GLOBALS['fxpglobals']['lang'];
	if($lang == 1)
	{
		$dsa['cfxbody']['name']		= "Hintergrund";
		$dsa['cfxbody']['desc']		= "Globaler durchgängiger Hintergrund (Wird überdeckt von der Navigationstoolbar, der Menütoolbar sowie des Arbeitsbereichs)";

		$dsa['ifxtoolc']['name']	= "Navigationstoolbar";
		$dsa['ifxtoolc']['desc']	= "Toolbarbereich ganz oben zum Aufrufen der wichtigsten Programmfunktionen wie Filter, Einstellungen, Hilfe etc.";

		$dsa['cticontxt']['name']	= "Navigations-Icons";
		$dsa['cticontxt']['desc']	= "Tools zum Aufrufen der wichtigsten Programmfunktionen wie Filter, Einstellungen, Hilfe etc.";

		$dsa['ifxsrcinput']['name']	= "Navigations-Suche";
		$dsa['ifxsrcinput']['desc']	= "Suchfeld zum Auflisten und Finden aller Programmfunktionen";

		$dsa['cttitle']['name']		= "Breadcrumb-Navigation";
		$dsa['cttitle']['desc']		= "Kompletter Pfad zur aktuell ausgeführten Programmfunktion";

		$dsa['cactreg']['name']		= "Aktion-Standard";
		$dsa['cactreg']['desc']		= "Aktionsbutton, welcher angelickt werden kann";

		$dsa['cactrega']['name']	= "Aktion-Aktiv";
		$dsa['cactrega']['desc']	= "Aktionsbutton des aktuell aktiven Aktionsmodus";

		$dsa['cactregi']['name']	= "Aktion-Inaktiv";
		$dsa['cactregi']['desc']	= "Aktionsbutton, welcher nicht angelickt werden kann";

		$dsa['ifxheaderc']['name']	= "Arbeitsbereich-Kopf";
		$dsa['ifxheaderc']['desc']	= "Kopfzeile des Arbeitsbereiches mit Buttons";

		$dsa['ifxwork']['name']		= "Arbeitsbereich-Hauptbereich";
		$dsa['ifxwork']['desc']		= "Hauptbereich des Arbeitsbereiches, in dem alle Masken angezeigt werden";

		$dsa['ifxfooterc']['name']	= "Arbeitsbereich-Fuss";
		$dsa['ifxfooterc']['desc']	= "Fusszeile des Arbeitsbereiches mit Zusatzinfos und Copyrightzeile";

		$dsa['cmaskh']['name']		= "Masken-Kopf";
		$dsa['cmaskh']['desc']		= "Kopfbereich der Maske mit Titel";

		$dsa['cmaskc']['name']		= "Masken-Inhalt";
		$dsa['cmaskc']['desc']		= "Inhaltsbereich der Maske";

		$dsa['cfxfbs']['name']		= "Button";
		$dsa['cfxfbs']['desc']		= "Button im Arbeitsbereich-Kopf zum Speichern oder innerhalb von Masken zur Auswählen etc.";

		$dsa['ifxtask']['name']		= "Taskbar";
		$dsa['ifxtask']['desc']		= "Informationszeile am unteren Rand zur Anzeige des Benutzer, der Spache etc.";

		$dsa['cmenu1-i']['name']	= "Standard-Menühintergrund";
		$dsa['cmenu1-i']['desc']	= "Standard-Hintergrundfarbe der 1. Menüebene";

		for($c=0; $c<=$ms; $c++)
		{
			$dsa['cmenu-c'.$c]['name']	= "Kategoriebereichsfarbe ".$c;
			$dsa['cmenu-c'.$c]['desc']	= "Hintergrundfarbe für Kategorie ".$c." Menüs";
		}
	}

	// Extract values from css
	if(strlen($css))
	{
		$search_debug=false;
		$css=str_replace(' ', '', $css);
if($search_debug) echo(__LINE__.':<pre>'.fxHtmlEncode($css).'</pre>');
		foreach($dsa as $dkey => $darr)
		{
			$d1=substr($dkey,0,1);
			if($d1 == 'h')						// HTML tag
				$ckey='dp_'.$dkey;
			else if(substr($dkey,0,1) == 'i')	// ID
				$ckey='#dp_'.$dkey;
			else								// Class
				$ckey='.dp_'.$dkey;
if($search_debug) echo('<hr />Search for &quot;'.$dkey.'&quot; &rarr; &quot;<b>'.$ckey.'</b>&quot; ...<br />');

			$cka=array($ckey.'{', $ckey.',', $ckey.':');
			foreach($cka as $ck)
			{
				$so=0;
				while(true)
				{
					$lp=strpos($css,$ck,$so);
					if($lp === false)
						break;

					$so=$lp+strlen($ck);

					$ls=strrpos(substr($css,0,$lp),"\n");
					if($ls === false)
						$ln=trim($css);
					else
						$ln=trim(substr($css,$ls));
					$le=strpos($ln,"\n");
					if($le !== false)
						$ln=trim(substr($ln,0,$le));
					if(strlen($ln))
					{
						$cs=strpos($ln,'{');
						if($cs !== false)
							$ln=trim(substr($ln,$cs+1));
						$ce=strpos($ln,'}');
						if($ce !== false)
							$ln=trim(substr($ln,0,$ce));
					}
if($search_debug) echo('... found at pos #<b>'.$lp.'</b> -- Line (#'.$ls.', len='.$le.'): <i class="lightblue">'.fxHtmlEncode($ln).'</i><br />');

					if(strlen($ln))
					{
						foreach($darr as $skey => $svalue)
						{
							if(($skey != 'name') && ($skey != 'desc') && ($svalue !== false))
							{
								$hover='';
								if(substr($skey,-6) == ':hover')
									$hover=':hover';
								if((!strlen($hover) && (substr($ck,-1) != ':')) || (strlen($hover) && (substr($ck,-1) == ':')))
								{
if($search_debug) echo('...... Subsearch for &quot;'.$skey.'&quot; <i class="grey">(hover=&quot;'.$hover.'&quot;)</i> ...<br />');
									$ln=strtolower($ln);
									if(strlen($hover))
										$sk=substr($skey,0,-6);
									else
										$sk=$skey;
									if($sk == 'background*')
										$ska=array('background:linear', 'background:');
									else if($sk == 'border-color')
										$ska=array('border-color:', 'border-left-color:', 'border-top-color:', 'border-right-color:', 'border-bottom-color:', 'border:', 'border-left:', 'border-top:', 'border-right:', 'border-bottom:');
									else
										$ska=array($sk.':');
									foreach($ska as $skc)
									{
										$sp=strpos($ln,$skc);
										if($sp !== false)
										{
											$nv='transparent';
											$sv=trim(substr($ln,$sp+strlen($skc)));
											$cp=strpos($sv,';');
											if($cp !== false)
												$sv=trim(substr($sv,0,$cp));
											$bp=strpos($sv,'(');
											if($bp !== false)
												$sv=trim(substr($sv,$bp+1));
											$bp=strpos($sv,')');
											if($bp !== false)
												$sv=trim(substr($sv,0,$bp));
											if(substr($sk,-1) == '*')	// Gradient
											{
												$nv='';
												$cc=0;
												$gr=2;
												$sa=explode(',', $sv);
												foreach($sa as $sac)
												{
													if(substr($sac,0,1) == '#')
													{
														$cc++;
														if(strlen($nv))
															$nv .= ',';
														$nv .= formatHTMLColor($sac, true);
													}
													else if(substr($sac,0,2) == 'to')
														$gr=fxpGetGradientCSS($sac);
												}
												if(($cc > 1) && ($gr != 2))
													$nv=$gr.'|'.$nv;
											}
											else
											{
												$hp=strpos($sv,'#');
												if($hp !== false)
													$nv=formatHTMLColor(trim(substr($sv,$hp)), true);
											}
if($search_debug) echo('......... found at pos #<b>'.$sp.'</b> -- <i class="blue">'.fxHtmlEncode($sv).'</i> &rarr; <b>$dsa[\''.$dkey.'\'][\''.$skey.'\']</b> = <b class="green">'.fxHtmlEncode($nv).'</b><br />');
											$dsa[$dkey][$skey]=$nv;
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Eliminate empty menu colors
	$mca=array();
	for($c=0; $c<=$ms; $c++)
	{
		$k='cmenu-c'.$c;
		if(strlen($dsa[$k]['background']) && ($dsa[$k]['background'] != 'transparent'))
			$mca[]=$dsa[$k]['background'];
	}
	for($c=0; $c<=$ms; $c++)
	{
		$k='cmenu-c'.$c;
		if(isset($mca[$c]))
			$dsa[$k]['background']=$mca[$c];
		else
			$dsa[$k]['background']='';
	}

	return $dsa;
}

/**
 * Create a new CSS client design
 *
 * @param various $css_name - Mandatory parameter:                  Name of the client design
 * @param various $dsa      - Mandatory parameter:                  Client design array
 * @param integer $ms       - Optional parameter (default = 0):     Number of menu colors to be used (if 0 all are used)
 * @param boolean $with_mcc - Optional parameter (default = true):  With menu category colors (true) or only basic colors (false)
 *
 * @return Client design CSS
 */
function fxpCreateClientCSS($css_name,$dsa,$ms=0,$with_mcc=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$css_name=strtr($css_name, array('*'=>'', '/'=>'', '\\'=>''));

	// Get all colors
	$set_debug=false;
	$m1a=array();
	$m1c=0;
	foreach($dsa as $dkey => $sa)
	{
		foreach($sa as $skey => $svalue)
		{
			if(($skey != 'name') && ($skey != 'desc'))
			{
				// Unset values
				if(isset($vvalue))
					unset($vvalue);
				$v=0;
				while(isset(${'vvalue'.$v}))
				{
					unset(${'vvalue'.$v});
					$v++;
				}
				$vmax=2;

				$vname=strtr($dkey.'_'.$skey, array('*'=>'', '-'=>'', ':'=>''));
				if(strlen($svalue))
				{
//fxDebug($svalue,'$svalue: $skey='.$skey, 0);
					$ci=2;
					$cd='bottom';
					$ca='&darr;';
					if(substr($svalue,1,1) == '|')
					{
						$cda=fxpGetGradientDirection(substr($svalue,0,1));
						if(sizeof($cda) == 3)
						{
							$ci=$cda[0];
							$cd=$cda[1];
							$ca=$cda[2];
						}
						$svalue=substr($svalue,2);
					}
					$va=explode(',',$svalue);
					$vmax=sizeof($va);
//fxDebug($va,'$va: $skey='.$skey.', $cd='.$cd, 0);

					if($vmax < 2)
						$vvalue=formatHTMLColor($va[0], true);
					else
					{
						$vvalue='background:linear-gradient(to '.$cd;
						foreach($va as $v => $val)
						{
							$val=formatHTMLColor($val, true);
							${'vvalue'.$v}=$val;
							$vvalue .= ','.$val;
						}
						$vvalue=getInterpolatedHTMLColor(0.5, ${'vvalue0'}, ${'vvalue'.$v}, true).';'.$vvalue.')';
					}
				}

				if(isset($vvalue))
					$GLOBALS[$vname]=$vvalue;
				else
					$GLOBALS[$vname]='transparent';
if($set_debug) echo('&middot; $GLOBALS[\''.$vname.'\'] = <b>'.$GLOBALS[$vname].'</b><br />');
				$lvalue=$GLOBALS[$vname];
				for($v=0; $v<$vmax; $v++)
				{
					if(isset(${'vvalue'.$v}))
					{
						$GLOBALS[$vname.'_'.$v]=${'vvalue'.$v};
						$lvalue=$GLOBALS[$vname.'_'.$v];
					}
					else
						$GLOBALS[$vname.'_'.$v]=$lvalue;
if($set_debug) echo('&middot; $GLOBALS[\''.$vname.'_'.$v.'\'] = <b>'.$GLOBALS[$vname.'_'.$v].'</b><br />');
				}
				$GLOBALS[$vname.'_last']=$lvalue;
if($set_debug) echo('&middot; $GLOBALS[\''.$vname.'_last\'] = <b>'.$GLOBALS[$vname.'_last'].'</b><br />');

				// Menu colors
				if((substr($dkey,0,7) == 'cmenu-c') && ((int)substr($dkey,7) > 0) && strlen($GLOBALS[$vname]) && ($GLOBALS[$vname] != 'transparent'))
				{
					$m1a[$m1c]=$GLOBALS[$vname];
					$m1c++;
				}
			}
		}
	}

	// Interpolate all menu colors
	$ms=(int)$ms;
	if(!$ms)
	{
		$ma=loadMenu(true);
		foreach($ma as $id => $sa)
		{
			if(!$sa['lv'] && ($sa['tx'] != 'Sysadmin'))
				$ms++;
		}
	}
	$mca=getMenuColors($ms, $GLOBALS['cmenu1i_background'], $m1a);
//fxDebug($mca,'$mca: cmenu1-i='.$GLOBALS['cmenu1i_background']);

	// Create stylesheet with new colors
	$s  = '/'.'*'.' '.$css_name.' '.'*'.'/'.$GLOBALS['nl'];
	$s .= 'dp_hbody, .dp_cfxbody {background:'.$GLOBALS['cfxbody_background'].';color:'.$GLOBALS['cfxbody_color'].';}'.$GLOBALS['nl'];
	$s .= '#dp_ifxtoolc, .dp_cfxtoolc {background:'.$GLOBALS['ifxtoolc_background'].';border-bottom:1px solid '.$GLOBALS['ifxtoolc_borderbottom'].';}'.$GLOBALS['nl'];
	$s .= '#dp_ifxtoola {background:'.$GLOBALS['ifxtoolc_background_last'].';border-right:1px solid '.$GLOBALS['ifxtoolc_borderbottom'].';border-bottom:1px solid '.$GLOBALS['ifxtoolc_borderbottom'].';}'.$GLOBALS['nl'];
	$s .= '.dp_cttitle {color:'.$GLOBALS['cttitle_color'].';}'.$GLOBALS['nl'];
	$s .= '.dp_cticontxt {color:'.$GLOBALS['cticontxt_color'].';}'.$GLOBALS['nl'];

	// ...Base menu colors
	$s .= '.dp_cmenu-c0 {background:'.$mca[1][0][0]['background'].';}'.$GLOBALS['nl'];
	for($m=1; $m<=$ms; $m++)
	{
		$v='';
		if(isset($m1a[$m-1]) && strlen($m1a[$m-1]))
			$v='background:'.$m1a[$m-1].';';
		$s .= '.dp_cmenu-c'.$m.' {'.$v.'}'.$GLOBALS['nl'];
	}

	// ...Menu category colors
	if($with_mcc)
		$s .= getMenuColorsCSS($ms, $mca, true);
	else
		$s .= '.dp_cmenu1-i {background:'.$mca[0]['background'].';color:'.$mca[0]['color'].';}'.$GLOBALS['nl'];

	// ...Rest
	$s .= '#dp_ifxsrcinput {background:'.$GLOBALS['ifxsrcinput_background'].';}'.$GLOBALS['nl'];
	$s .= '#dp_ipjsmt_src {background:#eeeeee;}'.$GLOBALS['nl'];
	$s .= '#dp_ifxtask {background:'.$GLOBALS['ifxtask_background'].';color:'.$GLOBALS['ifxtask_color'].';border-top:1px solid '.$GLOBALS['ifxtask_bordertop'].';}'.$GLOBALS['nl'];
	$s .= '.dp_cactreg {background:'.$GLOBALS['cactreg_background'].';color:'.$GLOBALS['cactreg_color'].';}'.$GLOBALS['nl'];
	$s .= '.dp_cactrega {background:'.$GLOBALS['cactrega_background'].';color:'.$GLOBALS['cactrega_color'].';}'.$GLOBALS['nl'];
	$s .= '.dp_cactregi {color:'.$GLOBALS['cactregi_color'].';}'.$GLOBALS['nl'];
	$s .= '#dp_ifxheaderc, .dp_cfxheaderc {background:'.$GLOBALS['ifxheaderc_background'].';color:'.$GLOBALS['ifxheaderc_color'].';}'.$GLOBALS['nl'];
	$s .= '#dp_ifxheader, .dp_cfxheader, .dp_cfxheadertce {color:'.$GLOBALS['ifxheaderc_color'].';}'.$GLOBALS['nl'];
	$s .= '#dp_ifxworkf, #dp_ifxwork, .dp_cfxworkf, .dp_cfxwork {background:'.$GLOBALS['ifxwork_background'].';color:'.$GLOBALS['ifxwork_color'].';}'.$GLOBALS['nl'];
	$s .= '#dp_ifxworkf, .dp_cfxworkf {border-top:1px solid '.$GLOBALS['ifxwork_bordertop'].';}'.$GLOBALS['nl'];
	$s .= '#dp_ifxfooterc, .dp_cfxfooterc {background:'.$GLOBALS['ifxfooterc_background'].';border:0;border-top:1px solid '.$GLOBALS['ifxfooterc_bordertop'].';border-bottom:1px solid '.$GLOBALS['ifxfooterc_borderbottom'].';}'.$GLOBALS['nl'];
	$s .= '.dp_cmaskf, .dp_cmaskc {background:'.$GLOBALS['cmaskc_background'].';color:'.$GLOBALS['cmaskc_color'].';}'.$GLOBALS['nl'];
	$s .= '.dp_cmaskc {border-left:1px solid '.$GLOBALS['cmaskc_borderleft'].';border-top:1px solid '.$GLOBALS['cmaskc_bordertop'].';border-right:1px solid '.$GLOBALS['cmaskc_borderright'].';border-bottom:1px solid '.$GLOBALS['cmaskc_borderbottom'].';}'.$GLOBALS['nl'];
	$s .= '.dp_cmaskh, .dp_cmaskt {background:'.$GLOBALS['cmaskh_background'].';color:'.$GLOBALS['cmaskh_color'].';border-bottom:1px solid '.$GLOBALS['cmaskh_borderbottom'].';}'.$GLOBALS['nl'];
	$s .= '.dp_cfxfbs, .dp_cfxfbr, .dp_cbtn, .dp_cdbtn, .dp_cfmaction, .dp_cfxfbsb, .dp_cfxfbrb, .dp_cfxfbb {background:'.$GLOBALS['cfxfbs_background'].';color:'.$GLOBALS['cfxfbs_color'].';border:2px outset '.$GLOBALS['cfxfbs_bordercolor'].';}'.$GLOBALS['nl'];
	$s .= '.dp_cfxfbs:hover, .dp_cfxfbr:hover, .dp_cbtn:hover, .dp_cdbtn, .dp_cfmaction:hover, .dp_cfxfbsb:hover, .dp_cfxfbrb:hover {color:'.$GLOBALS['cfxfbs_colorhover'].';border-color:'.$GLOBALS['cfxfbs_bordercolorhover'].';}'.$GLOBALS['nl'];

	return $s;
}

/**
 * Get gradient direction according to number block, e.g. 2 is "bottom"
 *
 * @param integer $c  - Optional parameter (default = 2):      Direction number according to number block (if not found, the default direction 2 = bottom is returned)
 * @param boolean $ra - Optional parameter (default = false):  Return the complete direction array or not
 *
 * @return Specific or Complete direction array
 */
function fxpGetGradientDirection($c=0, $ra=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$cda=array(
		1=>array(1, 'bottom left', '&swarr;'),
		2=>array(2, 'bottom', '&darr;'),
		3=>array(3, 'bottom right', '&searr;'),
		4=>array(4, 'left', '&larr;'),
		6=>array(6, 'right', '&rarr;'),
		7=>array(7, 'top left', '&nwarr;'),
		8=>array(8, 'top', '&uarr;'),
		9=>array(9, 'top right', '&nearr;')
	);

	if($ra)
		return $cda;

	$c=(int)$c;
	if(!isset($cda[$c]))
		$c=2;

	return $cda[$c];
}

/**
 * Get gradient direction style according to number block, e.g. "bottom" is 2
 *
 * @param integer $st - Optional parameter (default = 'bottom'):  Direction style, e.g. "bottom" or "tobottom" or "to bottom"
 * @param boolean $ra - Optional parameter (default = false):     Return the complete direction array or not
 *
 * @return Specific or Complete direction array
 */
function fxpGetGradientCSS($st='bottom', $ra=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$cda=array(
		1=>array(1, 'bottom left', '&swarr;'),
		2=>array(2, 'bottom', '&darr;'),
		3=>array(3, 'bottom right', '&searr;'),
		4=>array(4, 'left', '&larr;'),
		6=>array(6, 'right', '&rarr;'),
		7=>array(7, 'top left', '&nwarr;'),
		8=>array(8, 'top', '&uarr;'),
		9=>array(9, 'top right', '&nearr;')
	);

	if($ra)
		return $cda;

	$c=2;
	$st=trim($st);
	if(!strlen($st))
		return $c;

	$st=strtr(str_replace(' ', '', strtolower(trim($st))), array('tob'=>'b', 'tol'=>'l', 'tor'=>'r', 'tot'=>'t'));
	foreach($cda as $ca)
	{
		$sc=str_replace(' ', '', $ca[1]);
		if($st === $sc)
		{
			$c=$ca[0];
			break;
		}
	}

	return $c;
}

/**
 * Get background color to be used for masks, e.g. "f0f0f0" or "fafafa"
 *
 * @return Background color
 */
function fxpGetMaskBackground()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo($GLOBALS['fxpglobals']['design'].'<br />');
	$bg='f0f0f0';
	if(strlen($GLOBALS['fxpglobals']['design']))
		$bg='fafafa';

	return $bg;
}

/**
 * Get Multipurpose Internet Mail Extension Types
 *
 * @param string  $suffix - Optional parameter (default = ''):     Get only types with this suffix
 * @param boolean $ra     - Optional parameter (default = false):  Return result as text (false) or array (true)
 *
 * @return Multipurpose Internet Mail Extension Types
 */
function getMIMETypes($suffix='', $ra=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$suffix=strtolower(trim($suffix));
	$ct=array(
		'ace'	=> '00application/x-zip-compressed',
		'ai'	=> '00application/postscript',
		'bin'	=> '00application/octet-stream',
		'cab'	=> '00application/x-shockwave-flash',
		'chm'	=> '00application/mshelp',
		'cla'	=> '00application/octet-stream',
		'com'	=> '00application/octet-stream',
		'dll'	=> '00application/octet-stream',
		'doc'	=> '00application/msword',
		'docx'	=> '00application/msword',
		'dot'	=> '00application/msword',
		'dwg'	=> '00application/acad',
		'dxf'	=> '00application/dxf',
		'eps'	=> '00application/postscript',
		'exe'	=> '00application/octet-stream',
		'gta'	=> '00application/x-gtar',
		'gz'	=> '00application/gzip',
		'hlp'	=> '00application/mshelp',
		'mpp'	=> '00application/vnd.ms-project',
		'pdf'	=> '11application/pdf',
		'pot'	=> '00application/mspowerpoint',
		'pps'	=> '00application/mspowerpoint',
		'ppt'	=> '00application/mspowerpoint',
		'pptx'	=> '00application/mspowerpoint',
		'ppz'	=> '00application/mspowerpoint',
		'ps'	=> '00application/postscript',
		'rar'	=> '00application/x-zip-compressed',
		'rm'	=> '00application/vnd.rn-realmeadia',
		'rtc'	=> '00application/rtc',
		'rtf'	=> '00application/rtf',
		'swf'	=> '00application/x-shockwave-flash',
		'vmd'	=> '00application/vocaltec-media-desc',
		'vmf'	=> '00application/vocaltec-media-file',
		'xla'	=> '00application/msexcel',
		'xls'	=> '00application/msexcel',
		'xlsx'	=> '00application/msexcel',
		'z'		=> '00application/x-compress',
		'zip'	=> '00application/zip',

		'aif'	=> '01audio/x-aiff',
		'au'	=> '01audio/basic',
		'cht'	=> '01audio/x-dspeech',
		'dus'	=> '01audio/x-dspeech',
		'es'	=> '01audio/echospeech',
		'mid'	=> '01audio/x-midi',
		'mp2'	=> '01audio/x-mpeg',
		'mp3'	=> '01audio/x-mp3',
		'ogg'	=> '01audio/x-ogg',
		'ra'	=> '01audio/x-pn-realaudio',
		'ram'	=> '01audio/x-pn-realaudio',
		'rpm'	=> '01audio/x-pn-realaudio-plugin',
		'snd'	=> '01audio/basic',
		'str'	=> '01audio/x-qt-stream',
		'tsi'	=> '01audio/tsplayer',
		'vox'	=> '01audio/voxware',
		'wav'	=> '01audio/x-wav',

		'bmp'	=> '11image/bmp',
		'cod'	=> '11image/cis-cod',
		'dib'	=> '11image/bmp',
		'fif'	=> '11image/fif',
		'fh4'	=> '11image/x-freehand',
		'fh5'	=> '11image/x-freehand',
		'fhc'	=> '11image/x-freehand',
		'gif'	=> '11image/gif',
		'ico'	=> '11image/x-icon',
		'ief'	=> '11image/ief',
		'jpe'	=> '11image/jpeg',
		'jpg'	=> '11image/jpeg',
		'mcf'	=> '11image/vasa',
		'pct'	=> '11image/pict',
		'pic'	=> '11image/pict',
		'png'	=> '11image/png',
		'ras'	=> '11image/cmu-raster',
		'pnm'	=> '11image/x-portable-anymap',
		'pbm'	=> '11image/x-portable-bitmap',
		'pgm'	=> '11image/x-portable-graymap',
		'ppm'	=> '11image/x-portable-pixmap',
		'rgb'	=> '11image/x-rgb',
		'tif'	=> '11image/tiff',
		'wbm'	=> '11image/vnd.wap.wbmp',
		'xwd'	=> '11image/x-windowdump',
		'xbm'	=> '11image/x-xbitmap',
		'xpm'	=> '11image/x-xpixmap',

		'eml'	=> '11message/rfc822',
		'mht'	=> '11message/rfc822',
		'nws'	=> '11message/rfc822',

		'css'	=> '11text/css',
		'csv'	=> '11text/comma-separated-values',
		'htm'	=> '11text/html',
		'html'	=> '11text/html',
		'ifb'	=> '11text/calendar',
		'ics'	=> '11text/calendar',
		'js'	=> '11text/javascript',
		'jso'	=> '11text/json',
		'php'	=> '11text/html',
		'pht'	=> '11text/html',
		'plg'	=> '11text/html',
		'rtf'	=> '11text/rtf',
		'rtx'	=> '11text/richtext',
		'sh'	=> '11text/plain',
		'sht'	=> '11text/html',
		'spc'	=> '11text/x-speech',
		'sql'	=> '11text/sql',
		'tal'	=> '11text/x-speech',
		'text'	=> '11text/plain',
		'tsv'	=> '11text/tab-separated-values',
		'txt'	=> '11text/plain',
		'xml'	=> '11text/xml',
		'xsd'	=> '11text/xml-external-parsed-entity',
		'xsl'	=> '11text/xml',

		'asf'	=> '00video/x-ms-asf',
		'asx'	=> '00video/x-ms-asf',
		'avi'	=> '00video/x-msvideo',
		'mov'	=> '00video/quicktime',
		'mp4'	=> '01video/mpeg4',
		'mpe'	=> '00video/mpeg',
		'mpg'	=> '00video/mpeg',
		'mpv'	=> '00video/mpeg',
		'ogm'	=> '01video/ogg',
		'ogv'	=> '01video/ogg',
		'qt'	=> '00video/quicktime',
		'viv'	=> '00video/vnd.vivo',
		'webm'	=> '01video/webm',
		'wmv'	=> '00video/x-ms-wmv'
	);

	if(!strlen($suffix))
	{
		foreach($ct as $t => $m)
			$ct[$t]=substr($m,2);

		return $ct;
	}

	if(isset($ct[$suffix]))
	{
		$preview=false;
		if(substr($ct[$suffix],0,1) == '1')
			$preview=true;
		$display=false;
		if(substr($ct[$suffix],1,1) == '1')
			$display=true;
		$mime=substr($ct[$suffix],2);
		$sp=strpos($mime,'/');
		if(substr($mime,$sp+1) == 'html')
			$type='html';
		else
			$type=substr($mime,0,$sp);

		if($ra)
			return array('mime'=>$mime, 'type'=>$type, 'preview'=>$preview, 'display'=>$display);
		return $mime;
	}

	if($ra)
		return array('mime'=>"", 'type'=>"", 'preview'=>false, 'display'=>false);
	return '';
}

/**
 * Replace forbidden characters and prettify a filename
 *
 * @param various $fn - Mandatory parameter:  Filename
 *
 * @return Corrected and prettified filename
 */
function cleanFilename($fn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//$fn='ABC_abc [/] [\\] [ ] [>] [<] [:] [*] [?] ["] [|] [%] [%%] [%%%]';
//echo('cleanFilename: '.fxHtmlEncode($fn));

	// Replace (multiple) spaces with underscore
	$fn=preg_replace('/\040+/', '_', $fn);

	// Delete forbidden characters in filenames: [/] + [\] + [>] + [<] + [:] + [*] + [?] + ["] + [|]
	$fc='/[\/\\\\\>\<\:\*\?\"\|]/i';
	$fn=preg_replace($fc, '', $fn);

	// Replace multiple [%] with just one
	$fn=preg_replace('/\%\%+/', '%', $fn);
//echo(' -&gt; <b>'.fxHtmlEncode($fn).'</b><hr />');

	return $fn;
}

/**
 * Get all (interpolated) menu colors as an array
 *
 * @param various $ms  - Mandatory parameter:  Number of menu colors
 * @param various $m0  - Mandatory parameter:  Global menu color
 * @param various $m1a - Mandatory parameter:  Menu colors
 *
 * @return Menu colors
 */
function getMenuColors($ms, $m0, $m1a)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Menu global
	$m0=formatHTMLColor($m0, true);
	$m0hsv=getHSVColor($m0);

	$ma=array(
		0=>array('background'=>$m0, 'hue'=>$m0hsv['hue'], 'saturation'=>$m0hsv['saturation'], 'lightness'=>$m0hsv['lightness'], 'color'=>'#'.$m0hsv['color'], 'hover'=>'#'.$m0hsv['hover']),
		1=>array()
	);

	// Sysadmin menu colors
	$col='#000000';
	$m1hsv=getHSVColor($col);
	$ma[1][0]=array(
		0=>array('background'=>$col, 'hue'=>$m1hsv['hue'], 'saturation'=>$m1hsv['saturation'], 'lightness'=>$m1hsv['lightness'], 'color'=>'#'.$m1hsv['color'], 'hover'=>'#'.$m1hsv['hover'])
	);
	// ...Sub menus darker or lighter?
	$dcol='#f0f0f0';
	if($m1hsv['lightness'] >= 196)
		$dcol='#101010';
	for($a=1; $a<4; $a++)
	{
		$r=$a*0.25;
		$acol=getInterpolatedHTMLColor($r, $col, $dcol, true);
		$ahsv=getHSVColor($acol);
		$ma[1][0][$a]=array('background'=>$acol, 'hue'=>$ahsv['hue'], 'saturation'=>$ahsv['saturation'], 'lightness'=>$ahsv['lightness'], 'color'=>'#'.$ahsv['color'], 'hover'=>'#'.$ahsv['hover']);
	}

	// Menu colors
	if(!is_array($m1a))
		$m1a=array('#006b9f');
	$sm1=sizeof($m1a);
	if($sm1 < 2)
	{
		$m1a[1]=$m1a[0];
		$sm1++;
	}

	if($sm1 < 3)
		$s=$ms-1;						// 9+2: 8
	else
		$s=max(1.0,($ms-1)/($sm1-1));	// 9+3: 8/2=>4	9+4: 8/3=>2.66
//echo('$s='.$s.'<hr />');

	for($m=0; $m<$ms; $m++)
	{
		$d=$m/$s;			// 0=>0,    1=>0.33, 2=>0.66, 3=>1,    4=>1.33, 5=>1.66, 6=>2,    7=>2.33, 8=>2.66, 9=>3
		$csn=floor($d);		// 0=>0,    1=>0,    2=>0,    3=>1,    4=>1,    5=>1,    6=>2,    7=>2,    8=>2,    9=>3
		$cen=ceil($d);		// 0=>0,    1=>1,    2=>1,    3=>1,    4=>2,    5=>2,    6=>2,    7=>3,    8=>3,    9=>3
		$r=$d-$csn;			// 0=>0,    1=>0.33, 2=>0.66, 3=>0,    4=>0.33, 5=>0.66, 6=>0,    7=>0.33, 8=>0.66, 9=>0
//echo('$m='.$m.': $d='.$d.', $r='.$r.' -- $csn='.$csn.', $cen='.$cen.'<hr />');

		if(($csn == $cen) || ($sm1 == 1))	// Definition
			$col=formatHTMLColor($m1a[$csn], true);
		else					// Interpolation
		{
			$col=getInterpolatedHTMLColor($r, $m1a[$csn], $m1a[$cen], true);
//			$col='I'.$r.'('.$m1a[$csn].'-'.$m1a[$cen].') = '.$col;
		}
		$m1hsv=getHSVColor($col);
		$ma[1][$m+1]=array(
			0=>array('background'=>$col, 'hue'=>$m1hsv['hue'], 'saturation'=>$m1hsv['saturation'], 'lightness'=>$m1hsv['lightness'], 'color'=>'#'.$m1hsv['color'], 'hover'=>'#'.$m1hsv['hover'])
		);
		// ...Sub menus darker or lighter?
		$dcol='#f0f0f0';
		if($m1hsv['lightness'] >= 196)
			$dcol='#101010';
		for($a=1; $a<4; $a++)
		{
			$r=$a*0.25;
			$acol=getInterpolatedHTMLColor($r, $col, $dcol, true);
			$ahsv=getHSVColor($acol);
			$ma[1][$m+1][$a]=array('background'=>$acol, 'hue'=>$ahsv['hue'], 'saturation'=>$ahsv['saturation'], 'lightness'=>$ahsv['lightness'], 'color'=>'#'.$ahsv['color'], 'hover'=>'#'.$ahsv['hover']);
		}

	}

	return $ma;
}

/**
 * Get CSS menu colors
 *
 * @param integer $ms   - Optional parameter (default = 0):      Number of colors
 * @param boolean $mca  - Optional parameter (default = false):  Interpolated menu colors as an array (if false it will be created)
 * @param boolean $temp - Optional parameter (default = false):  Create for temporary preview use in relevant program function
 *
 * @return CSS menu colors
 */
function getMenuColorsCSS($ms=0, $mca=false, $temp=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$s='';
	$ms=(int)$ms;

	if(!$ms)
	{
		$ma=loadMenu();
//fxDebug($ma, '$ma');
		if(sizeof($ma))
		{
			foreach($ma as $mid => $marr)
			{
				if(!$marr['lv'] && ($marr['tx'] != 'Sysadmin'))
					$ms++;
			}
		}
	}

	if(!fxIsArray($mca))
	{
		$css=fxpCSSDesign();
//echo('<pre>'.$css.'</pre>');
		$dsa=fxpClientDesign($css,$ms);
//fxDebug($dsa,'$dsa: $css='.$css.', $ms='.$ms);
		$m0col=$dsa['cmenu1-i']['background'];
		$m1col=array();
		$c=1;
		if(fxIsArray($dsa))
		{
			while(fxIsArray($dsa['cmenu-c'.$c]))
			{
				if(isset($dsa['cmenu-c'.$c]['background']) && strlen($dsa['cmenu-c'.$c]['background']) && ($dsa['cmenu-c'.$c]['background'] != 'transparent'))
					$m1col[]=$dsa['cmenu-c'.$c]['background'];
				$c++;
			}
		}
//fxDebug($m1col,'$m1col');
		$mca=getMenuColors($ms, $m0col, $m1col);
//fxDebug($mca,'$mca');
	}

	// ...Menu category border color
	$dpc='.';
	if($temp)
		$dpc .= 'dp_c';
	$s .= $dpc.'menu1-bt {border-top:6px solid '.$mca[0]['background'].';}'.$GLOBALS['nl'];
	$s .= $dpc.'menu1-b {border-top-color:'.$mca[0]['background'].';}'.$GLOBALS['nl'];
	for($m=0; $m<=$ms; $m++)
	{
		$m2=substr('0'.$m,-2);

		$s .= $dpc.'menu1-'.$m2.'b {border-top-color:'.$mca[1][$m][0]['background'].';}'.$GLOBALS['nl'];
	}
	// ...Menu category colors for level 1
	$s .= $dpc.'menu1-sl {border-left:1px solid '.$mca[0]['color'].';}'.$GLOBALS['nl'];
	$s .= $dpc.'menu1-i {background:'.$mca[0]['background'].';color:'.$mca[0]['color'].';}'.$GLOBALS['nl'];
	for($m=0; $m<=$ms; $m++)
	{
		$m2=substr('0'.$m,-2);

		$s .= $dpc.'menu1-'.$m2.'a, '.$dpc.'menu1-'.$m2.':hover {background:'.$mca[1][$m][0]['background'].';color:'.$mca[1][$m][0]['hover'].';}'.$GLOBALS['nl'];
	}
	// ...Menu category colors for level 2
	for($m=0; $m<=$ms; $m++)
	{
		$m2=substr('0'.$m,-2);

		$s .= $dpc.'menu2-'.$m2.'-sl {border-left:1px solid '.$mca[1][$m][1]['color'].';}'.$GLOBALS['nl'];
		$s .= $dpc.'menu2-'.$m2.'i {background:'.$mca[1][$m][1]['background'].';color:'.$mca[1][$m][1]['color'].';}'.$GLOBALS['nl'];
		$s .= $dpc.'menu2-'.$m2.'a, '.$dpc.'menu2-'.$m2.':hover {background:'.$mca[1][$m][2]['background'].';color:'.$mca[1][$m][2]['hover'].';}'.$GLOBALS['nl'];
	}
	// ...Menu category colors for level 3
	for($m=0; $m<=$ms; $m++)
	{
		$m2=substr('0'.$m,-2);

		$s .= $dpc.'menu3-'.$m2.'-sl {border-left:1px solid '.$mca[1][$m][2]['color'].';}'.$GLOBALS['nl'];
		$s .= $dpc.'menu3-'.$m2.'i {background:'.$mca[1][$m][2]['background'].';color:'.$mca[1][$m][2]['color'].';}'.$GLOBALS['nl'];
		$s .= $dpc.'menu3-'.$m2.'a, '.$dpc.'menu3-'.$m2.':hover {background:'.$mca[1][$m][3]['background'].';color:'.$mca[1][$m][3]['hover'].';}'.$GLOBALS['nl'];
	}
	return $s;
}

/**
 * Return a 6 character long formatted html color
 *
 * @param various $col    - Mandatory parameter:                   Html color string
 * @param boolean $prefix - Optional parameter (default = false):  Return color with Hashtag (true) or not (false)
 *
 * @return Formatted html color
 */
function formatHTMLColor($col, $prefix=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$col=trim($col);
	if(substr($col,0,1) == '#')
		$col=substr($col,1);
	if(ctype_xdigit($col) && ((strlen($col) == 3) || (strlen($col) == 6)))
	{
		if(strlen($col) == 3)
			$col=substr($col,0,1).substr($col,0,1).substr($col,1,1).substr($col,1,1).substr($col,2,1).substr($col,2,1);
		if($prefix)
			$col='#'.$col;
	}

	return $col;
}

/**
 * Get RGB color of html color, e.g. #006b9f -> 27551
 *
 * @param string  $col - Mandatory parameter:                 Html color value
 * @param boolean $ra  - Optional parameter (default=false):  Return as array
 *
 * @return RGB color
 */
function getRGBColor($col, $ra=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$col=formatHTMLColor($col);

	$r=hexdec(substr($col,0,2));
	$g=hexdec(substr($col,2,2));
	$b=hexdec(substr($col,4));

	if($ra)
		return array('r'=>$r, 'g'=>$g, 'b'=>$b);
	return $b + ($g << 0x8) + ($r << 0x10);
}

/**
 * Get hue, saturation, lightness and text color informations of a html color in an array
 *
 * @param various $col - Mandatory parameter:  Html color value
 *
 * @return HSV information array
 */
function getHSVColor($col)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$rgb=getRGBColor($col);

	$r=0xFF & ($rgb >> 0x10);
	$g=0xFF & ($rgb >> 0x8);
	$b=0xFF & $rgb;

	$fr=((float)$r) / 255.0;
	$fg=((float)$g) / 255.0;
	$fb=((float)$b) / 255.0;

	$maxC=max($fr, $fg, $fb);
	$minC=min($fr, $fg, $fb);

	$l=($maxC+$minC) / 2.0;

	if($maxC == $minC)
	{
		$s=0;
		$h=0;
	}
	else
	{
		if($l < 0.5)
		{
			$s=($maxC-$minC) / ($maxC+$minC);
		}
		else
		{
			$s=($maxC-$minC) / (2.0-$maxC-$minC);
		}
		if($fr == $maxC)
			$h=($fg-$fb) / ($maxC-$minC);
		if($fg == $maxC)
			$h=2.0 + ($fb-$fr) / ($maxC-$minC);
		if($fb == $maxC)
			$h=4.0 + ($fr-$fg) / ($maxC-$minC);

		$h=$h / 6.0; 
	}

	$h=(int)round(255.0 * $h);
	$s=(int)round(255.0 * $s);
	$l=(int)round(255.0 * $l);

	// Text colors
	$tc='000000';
	if($l < 85)
		$tc='ffffff';
	$th='ffffff';
	if($l > 170)
		$th='000000';

	return array('hue'=>$h, 'saturation'=>$s, 'lightness'=>$l, 'color'=>$tc, 'hover'=>$th);
}

/**
 * Get interpolated color of two html colors
 *
 * @param double  $pc     - Mandatory parameter:                   Percentage of adjustment from start to end color: 0-1
 * @param string  $scol   - Mandatory parameter:                   Start html color
 * @param string  $ecol   - Mandatory parameter:                   End html color
 * @param boolean $prefix - Optional parameter (default = false):  Return color with Hashtag (true) or not (false)
 *
 * @return Interpolated html color
 */
function getInterpolatedHTMLColor($pc, $scol, $ecol, $prefix=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$scol=formatHTMLColor($scol);
	$ecol=formatHTMLColor($ecol);

	$s=array(hexdec(substr($scol,0,2)), hexdec(substr($scol,2,2)), hexdec(substr($scol,4)));
	$e=array(hexdec(substr($ecol,0,2)), hexdec(substr($ecol,2,2)), hexdec(substr($ecol,4)));

	$r=max(0, min(255, round($s[0] - ($s[0]-$e[0])*$pc)));
	$g=max(0, min(255, round($s[1] - ($s[1]-$e[1])*$pc)));
	$b=max(0, min(255, round($s[2] - ($s[2]-$e[2])*$pc)));

	$col=substr('0'.dechex($r),-2).substr('0'.dechex($g),-2).substr('0'.dechex($b),-2);
	if($prefix)
		$col='#'.$col;
//echo('<b>interpolateColor</b>($pc='.$pc.', $scol='.$scol.', $ecol='.$ecol.') -&gt; $r='.$r.', $g='.$g.', $b='.$b.' -&gt; '.$col.'<hr />');

	return $col;
}

/**
 * Get interpolated html color array from csv color string
 *
 * @param string  $cst    - Mandatory parameter:                   Color csv string
 * @param integer $size   - Optionale parameter (default = 0):     Size of interpolated entries in array (if 0 the size is determined from the csv string)
 * @param boolean $prefix - Optional parameter (default = false):  Return colors in array with Hashtag (true) or not (false)
 *
 * @return Interpolated html color array
 */
function getInterpolatedHTMLColorArray($cst, $size=0, $prefix=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$car=array();
	$cst=trim($cst);
	$size=(int)$size;
	if(strlen($cst))
	{
		$csa=explode(',', $cst);
		$csz=sizeof($csa);
		if($size <= 0)
			$size=$csz;
//fxDebug($csa,'$cst='.$cst.', $csz='.$csz.', $size='.$size, 0);
		for($c=0; $c<$size; $c++)
		{
			if(($csz == 1) || ($size <= 1))
				$gcl=formatHTMLColor($csa[0], $prefix);
			else
			{
				$gcs=floor(($csz-1)*$c/($size-1));
				$gcd=ceil(($csz-1)*$c/($size-1));
				$gpc=($c*($csz-1)/($size-1)) - $gcs;
				if(($gpc == 0.0) || ($gcs == $gcd))
					$gcl=formatHTMLColor($csa[$gcs], $prefix);
				else
					$gcl=getInterpolatedHTMLColor($gpc, $csa[$gcs], $csa[$gcd], $prefix);
//$gcl='Source='.$gcs.', Destination='.$gcd.', Percentage='.$gpc.'% -> '.$gcl;
			}
			$car[$c]=$gcl;
		}
	}

//fxDebug($car,'$car', 0);
	return $car;
}

/**
 * Write a progress status for javascript to load and use
 *
 * @param string $status   - Optional parameter (default = ''):  Status text to save (or load if empty)
 * @param string $filename - Optional parameter (default = ''):  Filename of the status file (if empty it will be created acording to the basename of the caller file)
 *
 * @return Save status: True on succes or false on error - Load status: Status text
 */
function fxProgressStatus($status='', $filename='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Status filename = adjusted basename of caller file, e.g. caller file = test.php results in _status_test.fst
	if(!strlen($filename))
	{
		if(isset($GLOBALS['__statusfilename']) && strlen($GLOBALS['__statusfilename']))
			$filename=$GLOBALS['__statusfilename'];
		else
		{
			$dbga=debug_backtrace();
			$filename='_status_'.strtolower(str_replace(' ','_',trim(basename($dbga[0]['file']))));
			$filename=substr($filename, 0, strrpos($filename,'.')).'.fst';
		}
	}

	$filepath='./'.$filename;
//echo('$filepath='.$filepath.'<br />$status='.$status.'<hr />');

	// Set status
	if(strlen($status))
	{
		fxSave($filepath, $status, true);
		if(file_exists($filepath))
			return true;
		return false;
	}

	// Get status, than delete file
	$status='';
	if(file_exists($filepath))
	{
		$status=fxLoad($filepath);
		fxDelete($filepath);
	}
//echo('&rarr; $status='.$status.'<hr />');
	return $status;
}

/**
 * Return a formatted system error or success message
 *
 * @param various $success - Mandatory parameter:                   Success (true) or Error (false)
 * @param boolean $dots    - Optional parameter (default = false):  Display three dots before the message (true) or not (false)
 * @param boolean $dbe     - Optional parameter (default = true):   Display database error messages after the message (true) or not(false)
 *
 * @return Formatted system message
 */
function sysStatus($success,$dots=false,$dbe=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$s='';
	if($dots)
		$s .= '&nbsp;<font style="color:#cccccc;">...</font>&nbsp;';
	$s .= '<b style="border:1px solid #cccccc;border-radius:4px;padding:0;margin:0;color:#';
	if($success)
		$s .= '009f6b;">&nbsp;OK';
	else
		$s .= 'e1001a;">&nbsp;ERROR';
	$s .= '&nbsp;</b>';

	if(!$success && $dbe)
	{
		$es=db_err();
		if(strlen($es))
			$s .= '<br /><hr class="fxhr"><font style="color:#e1001a;">'.db_err().'</font>';
	}

	return $s;
}

/**
 * Write worker log
 *
 * @param various $lline - Mandatory parameter:  Log line
 */
function tLog($lline)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$wfilename='';
	$ts=date('Y-m-d H:i:s',time());
	$trace=debug_backtrace();
	$tline=$trace[0]['line'];
	$tfile=$trace[0]['file'];
	$prefix=$ts."\t{".substr('00000'.(int)$_GET['lcnt'],-5).'-'.substr('000'.(int)$GLOBALS['fxpglobals']['tr'],-3)."}:\t".$tfile." [".substr('00000'.$tline,-5)."]\t";

	// Determine new line
	$lnl="\r\n";
	if(isset($_ENV) && (isset($_ENV['PATH']) && (substr($_ENV['PATH'], 0, 1) == '/') || (isset($_ENV['Path']) && substr($_ENV['Path'], 0, 1) == '/') || (isset($_ENV['path']) && substr($_ENV['path'], 0, 1) == '/')))
		$lnl="\n";

	// Start log?
	if($lline === '*START*')
	{
		$wfilename=$GLOBALS['logpath'].'_worker.log';
		$GLOBALS['fxptfilename']=$wfilename;
		$lline='';
		if(file_exists($wfilename))
			$lline=$lnl.$lnl;
		$lline .= '____________________________________________________________________________________________________________________________________________________________________________________________________________________________'.$lnl;
		$lline .= $prefix.'LOGSTART'.$lnl;
		$lline .= '°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°'.$lnl;
	}
	else if(isset($GLOBALS['fxptfilename']) && strlen($GLOBALS['fxptfilename']))
	{
		$wfilename=$GLOBALS['fxptfilename'];
		$lline=$prefix.$lline.$lnl;
	}

	if(strlen($wfilename))
		fxSave($wfilename, $lline, false);
}

/**
 * Get current program function (transaction)
 *
 * @return Transaction number
 */
function tGetTR()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$rtr=0;
	if(isset($GLOBALS['fxpglobals']) && is_array($GLOBALS['fxpglobals']) && sizeof($GLOBALS['fxpglobals']))
	{
		$filename=$GLOBALS['usrpath'].'C'.substr('0000'.$GLOBALS['fxpglobals']['client'],-4).'/P'.substr('000000000'.$GLOBALS['fxpglobals']['person'],-9).'/S'.$GLOBALS['lts'].'/t999.inc';
		if(file_exists($filename))
			require($filename);
		else
			$rtr=(int)$GLOBALS['fxpglobals']['tr'];
	}

	return $rtr;
}

/**
 * Save current program function (transaction)
 *
 * @param various $str - Mandatory parameter:  Current transaction number
 * @param various $rtr - Mandatory parameter:  Previous transaction nnumber
 */
function tSetTR($str,$rtr)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(isset($GLOBALS['fxpglobals']) && is_array($GLOBALS['fxpglobals']) && sizeof($GLOBALS['fxpglobals']))
	{
		$str=(int)$str;
		$rtr=(int)$rtr;
		$GLOBALS['fxpglobals']['tr']=$str;
		if($str != $rtr)
		{
			$filename=$GLOBALS['usrpath'].'C'.substr('0000'.$GLOBALS['fxpglobals']['client'],-4).'/P'.substr('000000000'.$GLOBALS['fxpglobals']['person'],-9).'/S'.$GLOBALS['lts'];
			if(is_dir($filename))
			{
				$filename .= '/t999.inc';
				fxSave($filename, '<'.'?'.'php $rtr='.$str.'; ?'.'>', true);
			}
		}
	}
}

/**
 * Get a formatted line number
 *
 * @param integer $ln - Mandatory parameter:  Line number
  *
 * @return Formatted line number
*/
function fxf_formatLineNumber($ln)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ln=(int)$ln;

	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$fl=substr('0000000'.$ln,-7);
	$fl=substr($fl,0,7-strlen($ln)).'<span style="color:#445;">'.substr($fl,7-strlen($ln)).'</span>';
	$fl='<span style="color:#bbc;">'.$fl.'</span>';

	return $fl;
}

/**
 * Determine the currently used browser from the HTTP_USER_AGENT
 * -> Sets the following global variables:
 *    - browser:       integer (0=Internet Explorer, 1=Opera, 2=Netscape Navigator, 3=Firefox, 4=Konqueror, 5=Safari, 6=Mozilla, 7=>Chrome, 8=Iron, 9=Mercury, 10=Internet Explorer: Edge, 11=>Microsoft Edge)
 *    - browservers:   string  (determined version number)
 *    - browsermobile: string  (empty, iPad, Android, Tablet, Mobile)
 *
 * @param string $val - Optional parameter (default = ''):  Name of the browser like "Firefox" (if empty, HTTP_USER_AGENT is used)
 *
 * @return Browser string containing name, version and mobile information
 */
function fxf_detBrowser($val='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$browsers=array(0=>'Internet Explorer', 1=>'Opera', 2=>'Netscape Navigator', 3=>'Firefox', 4=>'Konqueror', 5=>'Safari', 6=>'Mozilla', 7=>'Chrome', 8=>'Iron', 9=>'Mercury', 10=>'Internet Explorer: Edge', 11=>'Microsoft Edge');
	$browser_searcharr=array('1.1'=>'opera', '1.2'=>'opr', '1.3'=>'opios', '2.1'=>'netscape', '2.2'=>'navigator', '3'=>'firefox', '4'=>'konqueror', '9'=>'mercury', '8'=>'iron', '10'=>'edge', '11'=>'edg', '7.1'=>'chrome', '7.2'=>'crios', '5'=>'safari', '0.1'=>'msie', '0.2'=>'trident', '6'=>'mozilla');
//fxDebug($browser_searcharr, '$browser_searcharr');
	$GLOBALS['fxpglobals']['browser']=-1;
	$GLOBALS['fxpglobals']['browservers']='0';
	$GLOBALS['fxpglobals']['browsermobile']='';

	$val=trim($val);
	if(strlen($val))
		$browser_full=strtolower($val);
	else
		$browser_full=strtolower($_SERVER['HTTP_USER_AGENT']);
//echo('<pre>'.fxHtmlEncode($browser_full).'</pre>');

	foreach($browser_searcharr as $browsernr => $bsearchstring)
	{
		$bpos=strpos($browser_full,$bsearchstring);
//echo('Browser search for: '.$browsernr.' = '.$bsearchstring.' -- bpos='.$bpos.'<hr size=1 />');
		if($bpos !== false)
		{
			$GLOBALS['fxpglobals']['browser']=(int)$browsernr;
			$bposversion=strpos($browser_full,'version');
			if($bposversion)
				$bposversion += 7;
			if(!$bposversion)
			{
				$bposversion=strpos($browser_full,'rv:');
				if($bposversion)
					$bposversion += 3;
			}
			if(!$bposversion)
				$bposversion=$bpos+strlen($bsearchstring);
			$bposmax=strlen($browser_full);
			while(($bposversion < $bposmax) && !is_numeric(substr($browser_full,$bposversion,1)))
				$bposversion++;
			$GLOBALS['fxpglobals']['browservers']=substr($browser_full,$bposversion,1);
			$bposversion++;
			while(($bposversion < $bposmax) && (is_numeric(substr($browser_full,$bposversion,1)) || (substr($browser_full,$bposversion, 1) == '.')))
			{
				$GLOBALS['fxpglobals']['browservers'] .= substr($browser_full,$bposversion,1);
				$bposversion++;
			}
			break;
		}
	}

	// IE8 im Kompatibilitätsmodus?
	if(!$GLOBALS['fxpglobals']['browser'] && ($GLOBALS['fxpglobals']['browservers'] < '8.0'))
	{
		$tpos=strpos($browser_full,'trident/4.0');
		if($tpos)
			$GLOBALS['fxpglobals']['browservers']='8.0';
	}

	// Standardbrowser: Chrome 50.0
	if($GLOBALS['fxpglobals']['browser'] < 0)
	{
		$GLOBALS['fxpglobals']['browser']=7;
		$GLOBALS['fxpglobals']['browservers']='50.0';
	}
//echo('<b>'.$GLOBALS['fxpglobals']['browser'].'</b> &rarr; '.$browsers[$GLOBALS['fxpglobals']['browser']].' <i>(Version '.$GLOBALS['fxpglobals']['browservers'].')</i><hr size=1 />');

	// Mobile?
	$mobile_searcharr=array('iPad','Android','Tablet','Mobile');
	foreach($mobile_searcharr as $msearchstring)
	{
		$mpos=stripos($browser_full,$msearchstring);
		if($mpos !== false)
		{
			$GLOBALS['fxpglobals']['browsermobile']=$msearchstring;
			break;
		}
	}

	$bs=$browsers[$GLOBALS['fxpglobals']['browser']].' (Version '.$GLOBALS['fxpglobals']['browservers'].')';
	if(strlen($GLOBALS['fxpglobals']['browsermobile']))
		$bs .= ' / '.$GLOBALS['fxpglobals']['browsermobile'];
	return $bs;
}

/**
 * Check for or get the default ini filename and return result
 *
 * @param integer $get_default - Optional parameter (default = 0):  <=0: Check for the ini file,  1: Get ini path and name,  2:Get ini name,  >=3: Get ini path
 *
 * @return Ini filename (empty if not found and no default wanted)
 */
function fxf_fxIniFile($get_default=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// If you want your configuration file to be saved in a secure location change the next variable to a path your webserver can access, e.g. $inipath='/my/secure/location/';
	$inipath=$GLOBALS['mainpath'];
	// If you want a different name for your configuration file change the next variable, e.g. $ininame='fx_community.ini';
	$ininame='fx_project.ini';

	// Adjust path and name?
	$inipath=strtr(trim($inipath), array("\\\\"=>'/', "\\"=>'/'));
	if(substr($inipath,-1) != '/')
		$inipath .= '/';
	$ininame=trim($ininame);

	// Default wanted?
	$get_default=(int)$get_default;
	if($get_default > 0)
	{
		if($get_default == 1)
			return $inipath.$ininame;
		if($get_default == 2)
			return $ininame;
		return $inipath;
	}

	// Check for ini filename
	// ...Step 1: Ini path
	$filename=$inipath.$ininame;
	if(file_exists($filename))
		return $filename;
	// ...Step 2: Main path
	$filename=$GLOBALS['mainpath'].$ininame;
	if(file_exists($filename))
		return $filename;
	// ...Step 3: DAT subpath
	$filename=$GLOBALS['datpath'].$ininame;
	if(file_exists($filename))
		return $filename;
	// ...Step 4: Global os path
	if($GLOBALS['_os'] == 'windows')
		$filename='C:/'.$ininame;
	else
		$filename='/'.$ininame;
	if(file_exists($filename))
		return $filename;

	// Not found
	return '';
}

/**
 * Debug function: Runtime
 *
 * @param integer $mode - Mandatory parameter:                     0=Start, 1=Sub, 2=Difference to LastTime, 3=Difference to SubTime + 4=Difference to StartTime
 * @param string $cls   - Optional parameter (default = 'black'):  Color class for the type
 * @param string $type  - Optional parameter (default = ''):       Type to be displayed
 * @param string $text  - Optional parameter (default = ''):       Additional text to be displayed
 */
function fxf_dbgRuntime($mode, $cls='black', $type='', $text='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Runtime debug switched off - leave function
	if(($GLOBALS['__debug']['__DBG']&FXP_DEBUG_TIM) == 0)
		return;

	$dbbta=debug_backtrace();

	$mode=(int)$mode;
	if(($mode > 0) && (!isset($GLOBALS['_FXC']) || !isset($GLOBALS['_FXS']) || !fxIsArray($GLOBALS['_FXT'])))
		$mode=0;
	else
	{
		$GLOBALS['_FXC']++;
		$GLOBALS['_FXT'][$GLOBALS['_FXC']]=microtime_double();
	}

	$rtt='';

	$text=trim($text);
	if(strlen($text))
		$text='<div style="float:right;margin-right:8px;">&quot;'.$text.'&quot;</div>';

	switch($mode)
	{
		// Start
		case 0:
			$GLOBALS['_FXC']=0;
			$GLOBALS['_FXS']=0;
			$GLOBALS['_FXT']=array(0=>microtime_double());
			if(strlen($type))
				$rtt=$GLOBALS['_FXT'][$GLOBALS['_FXC']];
		break;

		// Sub
		case 1:
			$GLOBALS['_FXS']=$GLOBALS['_FXC'];
			if(strlen($type))
				$rtt=$GLOBALS['_FXT'][$GLOBALS['_FXC']];
		break;

		// Difference to SubTime
		case 3:
			if(!strlen($type))
				$type='&Delta; TOTAL-SUB';
			$rtt=$GLOBALS['_FXT'][$GLOBALS['_FXC']].' &rarr; <b class="'.$cls.'">'.($GLOBALS['_FXT'][$GLOBALS['_FXC']] - $GLOBALS['_FXT'][$GLOBALS['_FXS']]).' sec.</b>';
		break;

		// Difference to StartTime
		case 4:
			if(!strlen($type))
				$type='&Delta; TOTAL';
			$rtt=$GLOBALS['_FXT'][$GLOBALS['_FXC']].' &rarr; <b class="'.$cls.'">'.($GLOBALS['_FXT'][$GLOBALS['_FXC']] - $GLOBALS['_FXT'][0]).' sec.</b>';
		break;

		// Difference to LastTime
		default:
			if(!strlen($type))
				$type='RUNTIME #'.$GLOBALS['_FXC'];
			$rtt=$GLOBALS['_FXT'][$GLOBALS['_FXC']].' &rarr; <b class="'.$cls.'">'.($GLOBALS['_FXT'][$GLOBALS['_FXC']] - $GLOBALS['_FXT'][$GLOBALS['_FXC']-1]).' sec.</b>';
		break;
	}

	if(strlen($rtt))
	{
		$ds  = '<div class="dbgdl"><div style="float:left;margin-right:8px;"><img src="'.$GLOBALS['gfxpath'].'tim_b_16x16.png"></div>';
		$ds .= '<div class="lightergrey" style="float:left;width:320px;font-size:smaller;">'.$dbbta[0]['file'].' ('.$dbbta[0]['line'].')</div>';
		$ds .= '<div class="'.$cls.'" style="float:left;width:240px;font-weight:bold;">'.$type.'</div>';
		$ds .= '<div class="grey" style="float:left;">'.$rtt.'</div>';
		$ds .= $text.'</div>'.$GLOBALS['nl'];

		echo($ds);
	}
}

/**
 * Debug function: Memory
 *
 * @param string $cls  - Optional parameter (default = 'black'):  Color class for the type
 * @param string $type - Optional parameter (default = ''):       Type to be displayed
 * @param string $text - Optional parameter (default = ''):       Additional text to be displayed
 */
//function show_memory_usage($line=0, $file='', $display=false)
function fxf_dbgMemory($cls='black', $type='', $text='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Memory debug switched off - leave function
	if(($GLOBALS['__debug']['__DBG']&FXP_DEBUG_MEM) == 0)
		return;

	$dbbta=debug_backtrace();

	$type=trim($type);
	if(!strlen($type))
		$type='MEMORY USAGE';

	$text=trim($text);
	if(strlen($text))
		$text='<div style="float:right;margin-right:8px;">&quot;'.$text.'&quot;</div>';

	$ds  = '<div class="dbgdl"><div style="float:left;margin-right:8px;"><img src="'.$GLOBALS['gfxpath'].'mem_b_16x16.png"></div>';
	$ds .= '<div class="lightergrey" style="float:left;width:320px;font-size:smaller;">'.$dbbta[0]['file'].' ('.$dbbta[0]['line'].')</div>';
	$ds .= '<div class="'.$cls.'" style="float:left;width:240px;font-weight:bold;">'.$type.'</div>';
	$ds .= '<div class="grey" style="float:left;">'.round(memory_get_usage(true)/1048576, 2).' MB</div>';
	$ds .= $text.'</div>'.$GLOBALS['nl'];

	echo($ds);
}

/**
 * Debug function: Type
 *
 * @param integer $type - Mandatory parameter:                   Debug type, e.g. FXP_DEBUG_FRM for "Frame"
 * @param string $text  - Mandatory parameter:                   Main text to be displayed
 * @param string $note  - Optional parameter (default = ''):     Additional text to be displayed
 */
function fxf_dbgType($type, $text, $note='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Debug for this type switched off - leave function
	$type=(int)$type;
	if(($GLOBALS['__debug']['__DBG']&$type) == 0)
		return;

	$dbbta=debug_backtrace();

	$note=trim($note);
	if(strlen($note))
		$note='<div style="float:right;margin-right:8px;">'.$note.'</div>';

	$img='wfl';	// Frame
	if($type == FXP_DEBUG_FIL)	// File
		$img='por';

	$ds  = '<div class="dbgdl"><div style="float:left;margin-right:8px;"><img src="'.$GLOBALS['gfxpath'].$img.'_b_16x16.png"></div>';
	$ds .= '<div class="lightergrey" style="float:left;width:320px;font-size:smaller;">'.$dbbta[0]['file'].' ('.$dbbta[0]['line'].')</div>';
	$ds .= '<div class="darkgrey" style="float:left;">'.$text.'</div>';
	$ds .= $note.'</div>'.$GLOBALS['nl'];

	echo($ds);
}

/**
 * Debug function: Files
 *
 * @param string $filename   - Mandatory parameter:  Path and filename that should be checked
 * @param boolean $mandatory - Optional parameter:   If true file should exist else it is optional
 */
function fxf_dbgFiles($filename, $mandatory=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Frame and Files debug switched off - leave function
	if((($GLOBALS['__debug']['__DBG']&(FXP_DEBUG_FRM|FXP_DEBUG_FIL)) == 0))
		return;

	$dbbta=debug_backtrace();

	$ds  = '<div class="dbgdl"><div style="float:left;margin-right:8px;"><img src="'.$GLOBALS['gfxpath'].'por_b_16x16.png"></div>';
	$ds .= '<div class="lightergrey" style="float:left;width:320px;font-size:smaller;">'.$dbbta[0]['file'].' ('.$dbbta[0]['line'].')</div>';
	if(file_exists($filename))
		$ds .= '<div class="green" style="float:left;">File &quot;'.$filename.'&quot; included.</div>';
	else if($mandantory)
		$ds .= '<div class="red" style="float:left;">Check include file &quot;'.$filename.'&quot; ... <b class="red">Not found, but is mandatory!</b></div>';
	else
		$ds .= '<div class="darkgrey" style="float:left;">Check include file &quot;'.$filename.'&quot; ... <b>Not found!</b></div>';
	$ds .= '</div>'.$GLOBALS['nl'];

	echo($ds);
}

/**
 * Get an array with all client filenames (including pathes)
 *
 * @return Array with all client filenames
 */
function fxf_getClientFilenames()
{
	$l2=substr('0'.$GLOBALS['fxpglobals']['lang'],-2);

	$ca=array(
		'tmenu'		=> $GLOBALS['lngpath'].$l2.'-menu.txt',
		'gmenu'		=> $GLOBALS['usrpath'].'menu-'.$l2.'.fpi'
	);

	return $ca;
}

/**
 * Write progress log
 *
 * @param various $progress - Mandatory parameter:                ???PARAMETER???
 * @param various $message  - Mandatory parameter:                ???PARAMETER???
 * @param integer $pmax     - Optional parameter (default = 0):   ???PARAMETER???
 * @param integer $tline    - Optional parameter (default = 0):   ???PARAMETER???
 * @param string  $tfile    - Optional parameter (default = ''):  ???PARAMETER???
 */
function plog($progress, $message, $pmax=0, $tline=0, $tfile='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$tline || !strlen($tfile))
	{
		$trace=debug_backtrace();
		if(!$tline)
			$tline=$trace[0]['line'];
		if(!strlen($tfile))
			$tfile=$trace[0]['file'];
	}
	$log_filename=$GLOBALS['logpath'].'progress.log';

	$pmax=(int)$pmax;
	$progress=(int)$progress;

	// Logdaten in Datei schreiben
	if($progress >= 0)
	{
		$progress=max(0, min($pmax, $progress));

		$lc=0;
		$fp=false;
		while($lc < 20)
		{
			$se=fxSave($log_filename, $progress.'|'.$message.'|'.$pmax.'|'.$tline.'|'.$tfile);
			if(!$se)	// Save successful -> Leave loop
				break;
			usleep(250000);
			$lc++;
		}
	}
	else
		fxDelete($log_filename);
}

/**
 * Get javascript prefix
 *
 * @param string $fn - Optional parameter (default = ''):  Javascript function name to be called
 *
 * @return Javascript prefix string
*/
function fxf_jsFunctionPrefix($fn='')
{
	// Javascript function prefix
	$jp='fxf_fn_';
	if(strlen($fn) && (substr($fn,0,1) == '#'))
		$jp='fxf_en_';

	return $jp;
}

/**
 * Add a onclick javascript function call
 *
 * @param string  $fn         - Mandatory parameter:                   Javascript function name to be called
 *
 * @return onclick string or function name
*/
function fxf_jsFunction($fn, $na=false)
{
//	return $fn;

	// Javascript function prefix
	$jp=fxf_jsFunctionPrefix($fn);
	if(strlen($fn) && (substr($fn,0,1) == '#'))
		$fn=substr($fn,1);

	// Default mode = "onclick" (can be overwritten with an argument starting with "*on", e.g. "*onchange" changes mode to "onchange")
	$jm='onclick';

	// Call "Functionn"? (can be called with an argument starting with "^^", e.g. "^^fxf_fn_waitScreen();" calls function fxf_fn_waitScreen().)
	$jf='';

	// Call "waitScreen"? (can be called with an argument starting with "^", e.g. "^loading" calls function waitScreen('loading').)
	$jw='';

	// Include a delay? (can be included with an argument starting with ">", e.g. ">5" calls function xyz.delay(5, ...))
	$jd='';

	// Return only name if first argument is "!name"
	$on=false;

	// Arguments?
	$ja='';
	$fa=func_get_args();
//fxDebug($fa,'$fa: $fn='.$fn, 0);
	if(sizeof($fa) > 1)
	{
		foreach($fa as $fc => $fv)
		{
			if($fc)
			{
				// Return only name?
				if(strtolower($fv) == '!name')
				{
					$on=true;
					break;
				}
				// Overwrite onclick?
				else if(strtolower(substr($fv,0,3)) == '*on')
					$jm=strtolower(substr($fv,1));
				// Call "Function"?
				else if(substr($fv,0,2) == '^^')
					$jf=substr($fv,2).' ';
				// Call "waitScreen"?
				else if(substr($fv,0,1) == '^')
					$jw=$jp.'fxLinkClose(); '.$jp.'waitScreen(\''.substr($fv,1).'\'); ';
				else
				{
					if(strlen($ja))
						$ja .= ', ';
					if(is_string($fv) && (substr($fv,0,1) == '>'))
					{
						$jd='.delay';
						$ja .= substr($fv,1);
					}
					else if(is_string($fv) && (($fv != 'event') && (substr($fv,0,1) != '$')))
						$ja .= '\''.$fv.'\'';
					else if(is_bool($fv))
					{
						if($fv)
							$ja .= 'true';
						else
							$ja .= 'false';
					}
					else
						$ja .= $fv;
				}
			}
		}
	}

	// Return only name?
	if($on)
		$js=$jp.trim($fn);
	// Create call string
	else
		$js=$jm.'="'.$jf.$jw.$jp.trim($fn).$jd.'('.$ja.');"';
//echo('<pre>'.fxHtmlEncode($js).'</pre>');

	return $js;
}

/**
 * get_text
 * Diese Funktion liest aus der Tabelle "Texte" den angeforderten Text der
 * RefArt "Felder" in der aktuell eingestellten Sprache.
 * -----------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $ref:	ID_Referenz
 * $txt:	'kurz', 'lang', 'intern', 'text', '5' (STANDARD), '10', '15', '20'
 * $spr:	Sprach-ID oder 0 (STANDARD) -> Aktuell eingestellte Sprache
 * $rart:	ID_Ref_Art	1:Tabelle, 2:Maske, 3:Feld (STANDARD), 5:Funktion,
 * 6:Programmfunktion + 7:Maskengruppe
 * -----------------------------------------------------------------------------
 * Beispiel:
 * °°°°°°°°°
 * get_text(135, '10');
 * ->	liefert Literal_10 des Feldes 135 zurück -> "Auslastung"
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $ref  - Mandatory parameter:                 ???PARAMETER???
 * @param string  $txt  - Optional parameter (default = '5'):  ???PARAMETER???
 * @param integer $spr  - Optional parameter (default = 0):    ???PARAMETER???
 * @param integer $rart - Optional parameter (default = 3):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_text($ref, $txt='5', $spr=0, $rart=3)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$iref=(int)$ref;

	if(is_string($ref) && strlen($ref) && ($iref == 0))
	{
		$sql="SELECT id FROM felder WHERE ".db_like('feldname', $ref)." AND mandanten_id=0";
		$ref=(int)db_value($sql);
	}
	else
		$ref=$iref;

	if(!$ref)
		return false;

	$spr=(int)$spr;
	if(!$spr)
		$spr=$GLOBALS['fxpglobals']['lang'];

	$rart=(int)$rart;

	$txt=strtolower($txt);
	$tarray=array
	(
		'kurz'		=> 'text_kurz',
		'lang'		=> 'text_varlang',
		'intern'	=> 'anmerkungintern',
		'text'		=> 'literal_text',
		'5'			=> 'literal_5',
		'10'		=> 'literal_10',
		'15'		=> 'literal_15',
		'20'		=> 'literal_20'
   );

	$spalte=$tarray[$txt];
	if(!$spalte)
		$spalte=$tarray['5'];

	$tid=$spalte.'_'.$ref.'_'.$rart.'_'.$spr;

	$use_temp_text=true;
	if($spalte == 'text_varlang')
		$use_temp_text=false;

	if($use_temp_text && fxIsArray($GLOBALS['_temp_text']) && isset($GLOBALS['_temp_text'][$tid]))
		return $GLOBALS['_temp_text'][$tid];

	// Text aus Datenbank lesen (ID_Ref_Art=3 -> Felder)
	$sql="SELECT ".$spalte." FROM texte WHERE id_referenz=".$ref." AND id_ref_art=".$rart." AND id_sprache=".$spr;
	$text=apos2quot(db_value($sql));
//fxDebug($text,$sql, 0);

	if($use_temp_text && (!fxIsArray($GLOBALS['_temp_text']) || !isset($GLOBALS['_temp_text'][$tid])))
		$GLOBALS['_temp_text'][$tid]=$text;

	return $text;
}

/**
 * get_message
 * Diese Funktion liest aus der Tabelle "Meldungen" den angeforderten Text und
 * ersetzt alle Variablen der Form $var??? mit den übergebenen Werten. In der
 * Tabelle Meldungen können Memo-Texte eingegeben werden. Sinnvoll für lange
 * Texte, wie z.B. e-mails etc.
 * -----------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $id:		(integer)	-> Text_ID
 * (string)	-> Suchtext
 * $var1:	Wert, der anstelle von $var1 geschrieben wird
 * $var2:	Wert, der anstelle von $var2 geschrieben wird
 * ...
 * $var???:	Wert, der anstelle von $var??? geschrieben wird (beliebig gross)
 * -----------------------------------------------------------------------------
 * Beispiel:
 * °°°°°°°°°
 * get_message(1, 'Name', 5);
 * ->	liefert zurück: "Wir möchten Sie... Projekt: "Name" (PNr: 5)"
 * get_message('Projektentzug', 'Name', 5);
 * ->	liefert ebenfalls zurück: "Wir möchten Sie... Projekt: "Name" (PNr: 5)"
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $id - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_message($id)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$iid=(int)$id;
	$sid='';

	if(is_string($id) && strlen($id) && ($iid == 0))
		$sid=trim($id);

	if(!$iid && !strlen($sid))
		return false;

	// Sprache
	$msp=0;
	if(isset($GLOBALS['_lang_override']) && $GLOBALS['_lang_override'])
		$msp=(int)$GLOBALS['_lang_override'];
	if(!$msp)
		$msp=(int)$GLOBALS['fxpglobals']['lang'];

	// Mandanten-ID
	$mid=(int)$GLOBALS['fxpglobals']['client'];

	$sql  = "SELECT meldungstext FROM meldungen";
	$sql .=" WHERE (mandanten_id=".$mid." OR mandanten_id=0) AND id_sprache=".$msp." AND ";
	if(strlen($sid))
		$sql .= db_like('suchtext', $sid);
	else
		$sql .= "text_id=".$iid;
	$sql .= " ORDER BY mandanten_id DESC";
	$text=db_value($sql);

    $arg_list=func_get_args();
	$arg_cnt=sizeof($arg_list);
	if($arg_cnt > 1)
	{
		if(fxIsArray($arg_list[1]))
		{
			if(gettype(key($arg_list[1])) == 'string')
				$trans_table=$arg_list[1];
			else
			{
			    for($i=0; $i<sizeof($arg_list[1]); $i++)
					$trans_table['$var'.($i+1)]=$arg_list[1][$i];
			}
		}
		else
		{
		    for($i=1; $i<$arg_cnt; $i++)
				$trans_table['$var'.$i]=$arg_list[$i];
		}
		$text=strtr($text, $trans_table);
	}

	return $text;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $change      - Optional parameter (default = true): ???PARAMETER???
 * @param integer $only_client - Optional parameter (default = 0):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function usrLog($change=true,$only_client=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dbg=0;	// 0=Don't add debug info to file, 1=Add debug info to file
	$only_client=(int)$only_client;
	if($only_client)
		$change=false;

	$fnl="\r\n";

	$amt=microtime(true);									// Actual Unix timestamp (with microseconds)
	$aut=time();											// Actual Unix timestamp (in seconds)
	$aud=$aut-45;											// Possible time difference
//$aud=$aut-3600;
	$aft=date('YmdHis',$aut);								// Actual FeRox timestamp (from unix timestamp)
	if(fxIsArray($_GET,'kua'))
	{
		$acl=(int)$_GET['c'];								// Client ID
		$aui=(int)$_GET['u'];								// User ID
		$api=(int)$_GET['p'];								// Person ID
		$alt=substr($_GET['l'],0,14);						// Login timestamp
		$atr=(int)$_GET['t'];								// Transaction ID
		$anm=(string)$_GET['n'];							// Name
	}
	else
	{
		$acl=$GLOBALS['fxpglobals']['client'];				// Client ID
		$aui=$GLOBALS['fxpglobals']['user'];				// User ID
		$api=$GLOBALS['fxpglobals']['person'];				// Person ID
		$alt=substr($GLOBALS['lts'],0,14);					// Login timestamp
		$atr=$GLOBALS['fxpglobals']['tr'];					// Transaction ID
		$anm=$GLOBALS['fxpglobals']['persdat']['name1'];	// Name
	}
	if(!strlen($anm))
		$anm='{'.$api.'}';
	$pa=$acl.'-'.$aui.'-'.$api.'-'.$alt;

	$dbl=$amt.': '.$anm.' ('.$atr.'/'.$change.'/'.$only_client.')';

	// Load user file
	$u=array();
	$a=false;
	$f=$GLOBALS['usrpath'].'users.log';
	while(true)
	{
		if(file_exists($f))
		{
			$dbl .= ' [OK]';
			$a=file($f);
			if(is_array($a) && sizeof($a))
			{
				$dbl .= '['.sizeof($a).']';
				break;
			}
			else
			{
				$dbl .= '[0]';
				usleep(100);
			}
		}
		else
		{
			$dbl .= ' [KO]';
			break;
		}
	}
	if(is_array($a) && sizeof($a))
	{
		foreach($a as $l)
		{
			$l=trim($l);
			if(strlen($l))
			{
				$i=explode('|',$l);
				if(is_array($i) && (sizeof($i) > 7))
				{
					$iut=(int)$i[0];			// Unix timestamp
					$ift=(string)$i[1];			// FeRox timestamp
					$icl=(int)$i[2];			// Client ID
					$iui=(int)$i[3];			// User ID
					$ipi=(int)$i[4];			// Person ID
					$inm=(string)$i[5];			// Person name
					$ilt=substr($i[6],0,14);	// Login timestamp
					$itr=(int)$i[7];			// Transaction ID

					$pl=$icl.'-'.$iui.'-'.$ipi.'-'.$ilt;
					if(!$change || (($iut >= $aud) && ($itr > 0) && ($pl != $pa)))
					{
						$dbl .= ' ='.$inm;
						$u[$pl]=array('uts'=>$iut, 'fts'=>$ift, 'client'=>$icl, 'user'=>$iui, 'person'=>$ipi, 'name'=>$inm, 'lts'=>$ilt, 'tr'=>$itr);
					}
					else
					{
						$c='';
						if($iut < $aud)
							$c=' ('.$iut.' < '.$aud.')';
						else if($itr <= 0)
							$c=' ('.$itr.' <= 0)';
						else if($pl == $pa)
							$c=' ('.$pl.' == '.$pa.')';
						$dbl .= ' -'.$inm.$c;
					}
				}
			}
		}
	}

	// Add/change actual user
	if($change && ($atr > 0))
	{
		$dbl .= ' +'.$anm;
		$u[$pa]=array('uts'=>$aut, 'fts'=>$aft, 'client'=>$acl, 'user'=>$aui, 'person'=>$api, 'name'=>$anm, 'lts'=>$alt, 'tr'=>$atr);
	}
	$dbl .= '  ['.sizeof($u).']';
	if($change)
	{
		// Save user file
		if(sizeof($u))
		{
			$c='';
			foreach($u as $id => $d)
			{
				$l=$d['uts'].'|'.$d['fts'].'|'.$d['client'].'|'.$d['user'].'|'.$d['person'].'|'.str_replace('|', ' ', $d['name']).'|'.$d['lts'].'|'.$d['tr'];

				if(strlen($c))
					$c .= $fnl;
				$c .= $l;

				if($only_client && ($only_client != $d['client']))
					unset($u[$id]);
			}

			$fe=fxSave($f, $c);
			if($fe)	// Save error?
				$dbl .= ' {KO}';
			else
				$dbl .= ' {OK}';
		}
		else
		{
			fxDelete($f);
			$dbl .= ' {DEL}';
		}
	}
	else
		$dbl .= ' {NC}';

	if($dbg)
		fxSave($GLOBALS['usrpath'].'log-users.txt', $dbl.$fnl, false);

	return $u;
}

/**
 * ???FUNCTION???
 *
 * @param various $lmd - Mandatory parameter:                   ???PARAMETER???
 * @param various $mpi - Optional parameter (default = -1):     ???PARAMETER???
 * @param various $ast - Optional parameter (default = -1):     ???PARAMETER???
 * @param boolean $u   - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prjLog($lmd,$mpi=-1,$ast=-1,$u=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dbg=0;	// 0=Don't add debug info to file, 1=Add change debug info to file, 1=Add all debug info to file
	$lmd=strtolower(substr($lmd,0,1));	// Mode: w=write/u=update, r=read/g=get
	$ast=(int)$ast;

	$fnl="\r\n";

	if(($lmd == 'w') || ($lmd == 'r'))
		$nl="<br>\n";
	else
		$nl="\n";

	$su=-1;
	if(is_array($su))
		$su=sizeof($su);

	$amt=microtime(true);									// Actual Unix timestamp (with microseconds)
	$aut=time();											// Actual Unix timestamp (in seconds)
	$aud=$aut-25;											// Possible time difference
	$aft=date('YmdHis',$aut);								// Actual FeRox timestamp (from unix timestamp)
	if(fxIsArray($_GET,'kua'))
	{
		$acl=(int)$_GET['c'];								// Client ID
		$aui=(int)$_GET['u'];								// User ID
		$api=(int)$_GET['p'];								// Person ID
		$alt=substr($_GET['l'],0,14);						// Login timestamp
		$atr=(int)$_GET['t'];								// Transaction ID
		$anm=(string)$_GET['n'];							// Name
	}
	else
	{
		$acl=$GLOBALS['fxpglobals']['client'];				// Client ID
		$aui=$GLOBALS['fxpglobals']['user'];				// User ID
		$api=$GLOBALS['fxpglobals']['person'];				// Person ID
		$alt=substr($GLOBALS['lts'],0,14);					// Login timestamp
		$atr=$GLOBALS['fxpglobals']['tr'];					// Transaction ID
		$anm=$GLOBALS['fxpglobals']['persdat']['name1'];	// Name
	}
	if(!strlen($anm))
		$anm='{'.$api.'}';

	$dbl  = $amt.': prjLog($lmd='.$lmd.', $mpi='.$mpi.', $ast='.$ast.')'.$nl.$nl;

	// Load project file
	$c=false;
	$a=false;
	$p=array();
	$f=$GLOBALS['usrpath'].'projects.log';
	while(true)
	{
		if(file_exists($f))
		{
			$dbl .= ' [OK]';
			$a=file($f);
			if(is_array($a) && sizeof($a))
			{
				$dbl .= '['.sizeof($a).']';
				break;
			}
			else
			{
				$dbl .= '[0]';
				usleep(100);
			}
		}
		else
		{
			$dbl .= ' [KO]';
			break;
		}
	}
	if(is_array($a) && sizeof($a))
	{
		foreach($a as $l)
		{
			$l=trim($l);
			if(strlen($l))
			{
				$i=explode('|',$l);
				if(is_array($i) && (sizeof($i) > 20))
				{
					$iut=(int)$i[0];			// Unix timestamp
					$ift=(string)$i[1];			// FeRox timestamp
					$icl=(int)$i[2];			// Client ID

					$pid=(int)$i[3];			// Project ID
					$pno=(string)$i[4];			// Project No
					$pnm=(string)$i[5];			// Project name

					$cui=(int)$i[6];			// CO User ID
					$cpi=(int)$i[7];			// CO Person ID
					$cnm=(string)$i[8];			// CO Person name
					$clt=substr($i[9],0,14);	// CO Login timestamp
					$cct=(string)$i[10];		// CO Checkout timestamp
					$cft=(string)$i[11];		// CO FeRox timestamp
					$ctr=(int)$i[12];			// CO Transaction ID
					$cst=(int)$i[13];			// CO Status

					$rui=(int)$i[14];			// CO User ID
					$rpi=(int)$i[15];			// CO Person ID
					$rnm=(string)$i[16];		// CO Person name
					$rlt=substr($i[17],0,14);	// CO Login timestamp
					$rft=(string)$i[18];		// CO FeRox timestamp
					$rtr=(int)$i[19];			// CO Transaction ID
					$rst=(int)$i[20];			// CO Status

					$id=$icl.'-'.$pid;
					$p[$id]=array(
						'uts'		=> $iut,
						'fts'		=> $ift,
						'client'	=> $icl,

						'mpid'		=> $pid,
						'mpno'		=> $pno,
						'mpname'	=> $pnm,

					 	'cuser'		=> $cui,
					 	'cperson'	=> $cpi,
					 	'cname'		=> $cnm,
						'clts'		=> $clt,
						'ccts'		=> $cct,
						'cfts'		=> $cft,
						'ctr'		=> $ctr,
						'cstatus'	=> $cst,
						'cnstatus'	=> $cst,

					 	'ruser'		=> $rui,
					 	'rperson'	=> $rpi,
					 	'rname'		=> $rnm,
						'rlts'		=> $rlt,
						'rfts'		=> $rft,
						'rtr'		=> $rtr,
						'rstatus'	=> $rst,
						'rnstatus'	=> $rst
					);
				}
			}
		}
	}
	$dbl .= $nl.$nl.'$aut='.$aut.', $aud='.$aud.', $aft='.$aft.$nl;
	$dbl .= '$acl='.$acl.', $aui='.$aui.', $api='.$api.', $alt='.$alt.', $atr='.$atr.', $anm='.$anm.$nl;

	// Actual user
	$pa=$acl.'-'.$aui.'-'.$api.'-'.$alt;
	$dbl .= '[A] $pa='.$pa.$nl.$nl;

	$spo=sizeof($p);
	$dbl .= '$spo='.$spo.$nl.$nl;
//fxDebug($p,'$p: $spo='.$spo);

	// Write/Update log
	if((($lmd == 'w') || ($lmd == 'u')) && ($ast >= 0))
	{
		$cp=strpos($mpi,',');
		if($cp === false)
			$mpa=array(0=>(int)$mpi);
		else
			$mpa=explode(',',$mpi);
//fxDebug($mpa,'$mpa');

		$dbl .= 'Write/Update log: sizeof($mpa)='.sizeof($mpa).$nl;
		foreach($mpa as $mpi)
		{
			$mpi=(int)$mpi;
			if($mpi > 0)
			{
				$id=$acl.'-'.$mpi;
				$dbl .= '$id='.$id.', $ast='.$ast.$nl;
				if(!$ast)
				{
					$p[$id]=array(
						'uts'		=> $aut,
						'fts'		=> $aft,
						'client'	=> $acl,

						'mpid'		=> $mpi,
						'mpno'		=> '0',
						'mpname'	=> 'Dummy',

					 	'cuser'		=> $aui,
					 	'cperson'	=> $api,
					 	'cname'		=> $anm,
						'clts'		=> $alt,
						'ccts'		=> $aft,
						'cfts'		=> $aft,
						'ctr'		=> $atr,
						'cstatus'	=> $ast,
						'cnstatus'	=> $ast,

					 	'ruser'		=> 0,
					 	'rperson'	=> 0,
					 	'rname'		=> '',
						'rlts'		=> '',
						'rfts'		=> '',
						'rtr'		=> 0,
						'rstatus'	=> $ast,
						'rnstatus'	=> $ast
					);

					if(checkDB())
					{
						// Set project name
						$s="SELECT vorgangsnummer,name_projekt FROM projekte WHERE projekt_id=".$mpi." AND mandanten_id=".$acl;
						$v=db_values($s);
//fxDebug($v,$s);
						if(fxIsArray($v))
						{
							$p[$id]['mpno']		= (string)no_null($v['vorgangsnummer']);	// Project No
							$p[$id]['mpname']	= $v['name_projekt'];						// Project Name
						}
					}
					$dbl .= '- Set new [C]'.$nl;
					$c=true;
				}

				if(isset($p[$id]))
				{
					if($ast == 1)
					{
						$p[$id]['ruser']	= $aui;
						$p[$id]['rperson']	= $api;
						$p[$id]['rname']	= $anm;
						$p[$id]['rlts']		= $alt;
						$p[$id]['rfts']		= $aft;
						$p[$id]['rtr']		= $atr;
						$p[$id]['rnstatus']	= $ast;
						$dbl .= '- Set new [R]'.$nl;
						$c=true;
					}
					else if($ast > 1)
					{
						$p[$id]['cfts']		= $aft;
						$p[$id]['ctr']		= $atr;
						$p[$id]['cnstatus']	= $ast;
						$dbl .= '- Adjust [C]'.$nl;
						$c=true;
					}
				}
			}
		}
	}
	$spn=sizeof($p);
//fxDebug($p,'$p: $spn='.$spn);

	// Validations
	if($spn)
	{
		// Get active users
		if(!is_array($u))
			$u=usrLog(false);
		if($u === false)
			return '';
		$dbl .= $nl.'Users:'.$nl;
		if(sizeof($u))
		{
			foreach($u as $ui => $ud)
				$dbl .= $ui.$nl;
		}

		$dbl .= $nl.'Validations:'.$nl;
		foreach($p as $id => $d)
		{
			// [C] Person who checked out a project
			$pc=$d['client'].'-'.$d['cuser'].'-'.$d['cperson'].'-'.$d['clts'];
			// [R] Person who sent a request
			$pr=$d['client'].'-'.$d['ruser'].'-'.$d['rperson'].'-'.$d['rlts'];
			$dbl .= '$id='.$id.': [C] $pc='.$pc.' - [R] $pr='.$pr.' - [A] $pa='.$pa.$nl;

			$del=false;

			// No [R] or [R] does not exist anymore
			if(!$d['ruser'] || !isset($u[$pr]))
			{
				if(!$d['ruser'])
					$dbl .= '- No [R] ';
				else
					$dbl .= '- [R] does not exist anymore ';

				// [C] does not exists anymore
				if(!isset($u[$pc]))
				{
					$dbl .= 'and [C] does not exist anymore: Delete.'.$nl;
					$del=true;
				}
				else
				{
					$dbl .= 'but [C] exists: ';
					if($d['ruser'])
					{
						$dbl .= 'Reset [R] user. ';
						$p[$id]['ruser']=0;
						$p[$id]['rperson']=0;
						$p[$id]['rname']='';
						$p[$id]['rlts']='';
						$p[$id]['rfts']='';
						$p[$id]['rtr']=0;
						$p[$id]['rstatus']=0;
						$p[$id]['rnstatus']=0;
						$c=true;
					}
					if(($p[$id]['cnstatus'] > 0) && ($p[$id]['cnstatus'] < 98))
					{
						$dbl .= 'Request not relevant anymore - Reset [C] status to 0.'.$nl;
						$p[$id]['cnstatus']=0;
						$c=true;
					}
					else
						$dbl .= 'No [C] changes.'.$nl;
				}
			}
			// [C] does not exist anymore but [R] exists
			else if(!isset($u[$pc]))
			{
				$dbl .= '- [C] does not exist anymore but [R] exists: ';
				if($p[$id]['cnstatus'] < 98)
				{
					$dbl .= 'Request not relevant anymore - Set [C] status to 98.';
					$p[$id]['cnstatus']=98;
					$c=true;
				}
				else
					$dbl .= 'No [C] changes.';
				if(($pa == $pr)  && ($p[$id]['cnstatus'] != $p[$id]['rnstatus']))
				{
					$p[$id]['rnstatus']=$p[$id]['cnstatus'];
					$dbl .= ' Set Status of [R] to higher status of [C] = '.$p[$id]['cnstatus'].'.';
					$c=true;
				}
				$dbl .= $nl;
			}
			// Both [C] and [R] exist
			else
			{
				$dbl .= '- Both [C] and [R] exist';
				if(($pa == $pc) || ($pa == $pr))
				{
					if($pa == $pc)
						$dbl .= ', and [A]=[C]</b>:';
					else
						$dbl .= ', and [A]=[R]</b>:';
					if(($pa == $pc) && ($p[$id]['rnstatus'] > $p[$id]['cnstatus']))
					{
						$p[$id]['cnstatus']=$p[$id]['rnstatus'];
						$dbl .= ' Set Status of [C] to higher status of [R] = '.$p[$id]['rnstatus'].'.';
						$c=true;
					}
					if(($pa == $pr)  && ($p[$id]['cnstatus'] > $p[$id]['rnstatus']))
					{
						$p[$id]['rnstatus']=$p[$id]['cnstatus'];
						$dbl .= ' Set Status of [R] to higher status of [C] = '.$p[$id]['cnstatus'].'.';
						$c=true;
					}
					$dbl .= $nl;
				}
				else
					$dbl .= ', but [A] does not match any: No action.'.$nl;
				if(($p[$id]['cnstatus'] != $p[$id]['cstatus']) || ($p[$id]['rnstatus'] != $p[$id]['rstatus']))
					$c=true;
			}

			$dbl .= '- <b>Action</b>: ';
			if($del)
			{
				$dbl .= 'Check in MP ['.$d['mpid'].']. Unset entry ['.$id.'].'.$nl;
				$success=prjLogCheckIn($d['client'],$d['mpid'],$d['cperson'],$d['clts']);
				unset($p[$id]);
				$c=true;
			}
			else
			{
				$dbl .= 'cstatus='.$p[$id]['cstatus'].', cnstatus='.$p[$id]['cnstatus'].' / ruser='.$p[$id]['ruser'].', rstatus='.$p[$id]['rstatus'].', rnstatus='.$p[$id]['rnstatus'].':';
				if($p[$id]['cnstatus'] >= 98)
				{
					$dbl .= ' Check in MP ['.$d['mpid'].'].';
					$success=prjLogCheckIn($d['client'],$d['mpid'],$d['cperson'],$d['clts']);
					if(!$p[$id]['ruser'] || (($p[$id]['rstatus'] == $p[$id]['rnstatus']) && ($p[$id]['rnstatus'] >= $p[$id]['cnstatus'])))
					{
						$dbl .= ' Unset entry ['.$id.'].'.$nl;
						unset($p[$id]);
						$c=true;
					}
				}
				else
					$dbl .= ' (No action)'.$nl;
			}
		}
	}
	$spn=sizeof($p);
//fxDebug($p,'$p: $spn='.$spn);

	// Save projects file
	$dbl .= $nl.$nl.'$spo='.$spo.', $spn='.$spn.' - $c='.$c.$nl;
	if(($spo != $spn) || $c)
	{
		if(file_exists($f))
		{
			$dbl .= 'File ['.$f.'] exists... unlink'.$nl;
			@unlink($f);
		}
		if($spn)
		{
			$c='';
			foreach($p as $id => $d)
			{
				$l  = $d['uts'].'|'.$d['fts'];
				$l .= '|'.$d['client'].'|'.$d['mpid'].'|'.$d['mpno'].'|'.str_replace('|',' ',$d['mpname']);
				$l .= '|'.$d['cuser'].'|'.$d['cperson'].'|'.str_replace('|',' ',$d['cname']).'|'.$d['clts'].'|'.$d['ccts'].'|'.$d['cfts'].'|'.$d['ctr'].'|'.$d['cnstatus'];
				$l .= '|'.$d['ruser'].'|'.$d['rperson'].'|'.str_replace('|',' ',$d['rname']).'|'.$d['rlts'].'|'.$d['rfts'].'|'.$d['rtr'].'|'.$d['rnstatus'];

				if(strlen($c))
					$c .= $fnl;
				$c .= $l;

				$dbl .= 'Line with id='.$id.' written: '.$l.$nl;
			}

			$fe=fxSave($f, $c.$fnl);
			if(!$fe)	// Save OK?
				$dbl .= 'File ['.$f.'] written'.$nl;
		}
	}

	if(($lmd == 'r') || ($lmd == 'w'))
	{
//if($c) echo($dbl);
		if($dbg && ($c || ($dbg > 1)))
			fxSave($GLOBALS['usrpath'].'log-projects.txt', $dbl, false);
		return $p;
	}

	// User status message
	$dbl .= $nl.$nl.'Getting user status...'.$nl;
	$m='';
	if($spn)
	{
		foreach($p as $id => $d)
		{
			$vu='';
			$vi='';
			// [C] Person who checked out a project
			$pc=$d['client'].'-'.$d['cuser'].'-'.$d['cperson'].'-'.$d['clts'];
			$pr=$d['client'].'-'.$d['ruser'].'-'.$d['rperson'].'-'.$d['rlts'];
			$dbl .= '$id='.$id.': [C] $pc='.$pc.' - [R] $pr='.$pr.' - [A] $pa='.$pa.$nl;
			$dbl .= ' - cstatus='.$d['cstatus'].', cnstatus='.$d['cnstatus'].' - rstatus='.$d['rstatus'].', rnstatus='.$d['rnstatus'].$nl;
			if(($pa == $pc) && ($d['cstatus'] != $d['cnstatus']))
			{
				$vu='c';
				$vi='r';
			}
			// [R] Person who sent a request
			else if(($pa == $pr) && ($d['rstatus'] != $d['rnstatus']))
			{
				$vu='r';
				$vi='c';
			}

			$dbl .= '- vu='.$vu.', vi='.$vi.$nl;
			if(strlen($vu) && strlen($vi))
			{
				$m  = '#PL#'.$vu.'|'.$d['fts'].'|'.$d[$vu.'nstatus'].'|'.$d['mpid'].'|'.$d['mpno'].'|'.str_replace('|',' ',$d['mpname']).'|';
				$m .= $d[$vi.'user'].'|'.$d[$vi.'person'].'|'.str_replace('|',' ',$d[$vi.'name']).'|'.$d[$vi.'lts'].'|'.$d[$vi.'fts'].'|'.$d[$vi.'tr'];
				$c=true;
				break;
			}
		}
	}

	if(!strlen($m) && ($atr == 281))
	{
		$mo=(int)$_GET['m'];
		$mn=0;
		if($spn)
		{
			foreach($p as $id => $d)
			{
				if($d['client'] == $acl)
					$mn += $d['mpid'];
			}
		}
		if($mn != $mo)
			$m='#RL#';
	}
//if($c) echo($dbl);
	if($dbg && ($c || ($dbg > 1)))
		fxSave($GLOBALS['usrpath'].'log-projects.txt', $dbl, false);

	return $m;
}

/**
 * ???FUNCTION???
 *
 * @param various $client  - Mandatory parameter: ???PARAMETER???
 * @param various $mpid    - Mandatory parameter: ???PARAMETER???
 * @param various $cperson - Mandatory parameter: ???PARAMETER???
 * @param various $clts    - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function prjLogCheckIn($client,$mpid,$cperson,$clts)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!checkDB())
		return false;

	$s="SELECT cout_id,cout_date FROM projekte WHERE projekt_id=".$mpid." AND projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$client;
	$d=db_values($s);
//fxDebug($d,$s);
	if(fxIsArray($d) && ($d['cout_id'] == $cperson) && ($d['cout_date'] == $clts))
	{
		$sql  = "UPDATE projekte SET cout_id=NULL, cout_date=NULL";
		$sql .= " WHERE mutterprojekt_id=".$mpid." AND mandanten_id=".$client;
		$success=db_query($sql);
//fxDebug($success,$sql);
		return $success;
	}

	return true;
}

/**
 * ???FUNCTION???
 *
 * @param string  $idf    - Optional parameter (default = ''):    ???PARAMETER???
 * @param string  $order  - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $ordpos - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxf_getReferenceValues($idf='', $order='', $ordpos=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$idf=strtolower(trim($idf));
	switch($idf)
	{
		case 'priority':
		case 'prioritaet':
			$idf='47';
		break;

		case 'projectstatus':
		case 'projektstatus':
		case 'project_status':
		case 'projekt_status':
			$idf='137';
		break;

		case 'method':
		case 'methode':
			$idf='1659';
		break;

		case 'pointintime':
		case 'zeitpunkt':
			$idf='1660';
		break;

		case 'persontype':
		case 'personentyp':
			$idf='30';
		break;

		case 'language':
		case 'sprache':
			$idf='53';
		break;

		case 'day':
		case 'tag':
		case 'days':
		case 'tage':
		case 'weekday':
		case 'wochentag':
		case 'weekdays':
		case 'wochentage':
			$idf='289';
			$ordpos=true;
		break;

		case 'month':
		case 'monat':
		case 'months':
		case 'monate':
			$idf='62';
			$ordpos=true;
		break;

		case 'communicationtype':
		case 'kommunikationstyp':
			$idf='94';
		break;

		case 'category':
		case 'kategorie':
			$idf='1,10';
		break;
	}

	if(!strlen($idf))
		return '';

	$ie=explode(',',$idf);
	$ids='';
	foreach($ie as $i)
	{
		$ids .= ','.(int)$i;
		if((int)$i == 2049)	// Abrechnungsart_Mat -> Abrechnungsart
			$ids .= ',69';
	}
	$ids=substr($ids,1);

	$sql="SELECT mandanten_id, wertetabid, tabwert, position FROM wertetab WHERE mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND id_feld IN (".$ids.") AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND (archiv IS NULL OR archiv=0)";
	if(strlen($order))
		$sql .= db_orderby($order);
	else
		$sql .= db_orderby('position', 'tabwert');
	if($ordpos)
		$arr=get_array($sql, 'position', 'tabwert');
	else
		$arr=get_array($sql, 'wertetabid', 'tabwert');
//fxDebug($arr, $sql);

	return $arr;
}

/**
 * Create an array with the proper value for each bit
 *
 * @param integer $mb - Mandatory parameter:  Max bits
 *
 * @return Array of bit values
 */
function fxf_getBitArray($mb=8)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mb=max(1, min(128, (int)$mb));
	$ba=array();

	for($b=0; $b<$mb; $b++)
		$ba[$b]=pow(2,$b);

	return $ba;
}

/**
 * Wrap text after x letters, i.e. do a line break, or cut off text
 *
 * @param string  $text       - Mandatory parameter:                      Text
 * @param boolean $wrap       - Optional parameter (default = true):      Wrap text (= true) or cut off text (= false)
 * @param integer $maxz       - Optional parameter (default = 50):        Max. amount of letters in a line
 * @param string  $plstart    - Optional parameter (default = ''):        Add this text after a line break before each line
 * @param string  $plend      - Optional parameter (default = ''):        Add this text before a line break
 * @param string  $line_break - Optional parameter (default = '<br />'):  Do a line break with this text
 * @param boolean $plfirst    - Optional parameter (default = true):      Add the text before a line break also in the first line (= true) or not (= false)
 *
 * @return Adjusted text
 */
function text_wrap($text, $wrap=true, $maxz=50, $plstart='', $plend='', $line_break='<br />', $plfirst=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($text))
		return '';

	// Text aufbereiten
//echo('<div style="padding:6px;border:1px solid black;border-radius:4px;margin-bottom:8px;"><b>Text:</b> ($maxz='.$maxz.')<pre>'.fxHtmlEncode($text).'</pre></div>');
	$wrc='';
	if(is_string($wrap))
	{
		$wrc=$wrap;
		$wrap=true;
		if(substr($wrc,0,1) == '!')
		{
			$wrc=substr($wrc,1);
			$wrap=false;
		}
	}

	if(!$maxz)
	{
		$ptrans=array(chr(13).chr(10)=>chr(13).chr(10).$wrc, chr(10).chr(13)=>chr(10).chr(13).$wrc, chr(13)=>chr(13).$wrc, chr(10)=>chr(10).$wrc, '<BR>'=>'<br />'.$wrc, '<BR />'=>'<br />'.$wrc, '<br>'=>'<br />'.$wrc, '<br />'=>'<br />'.$wrc);
		$text=strtr($text,$ptrans);
//echo('<div style="padding:6px;border:1px solid black;border-radius:4px;margin-bottom:32px;"><b>Rückgabetext:</b><pre>'.fxHtmlEncode($text).'</pre></div>');
		return $text;
	}

	$rt='';
	$tse=explode('<br />', strtr($text, array('&quot;'=>'¤', '&nbsp;'=>'¬', '<BR>'=>'<br />', '<BR />'=>'<br />', '<br>'=>'<br />')));
//fxDebug($tse, '$tse');
	if(fxIsArray($tse))
	{
		foreach($tse as $tn => $tl)
		{
			$tl=trim($tl);
			while(substr($tl,0,1) == '¬')
				$tl=substr($tl,1);
			while(substr($tl,-1) == '¬')
				$tl=substr($tl,0,-1);
			$tl=trim($tl);

			$cl=strlen($tl);
			while($cl)
			{
				$it=-1;
				$tg='';
				$gt=false;
				if($cl <= $maxz)
				{
					$nt=$tl;
					$tl='';
				}
				else
				{
					$nt='';
					$bp=-1;
					$cp=0;
					$cc=0;
					while(true)
					{
						$c=substr($tl,$cp,1);
						$n='';
						if($cp+1 < $cl)
							$n=substr($tl,$cp+1,1);
						if(($it < 0) && ($c == '<') && ($n != ' '))
						{
							$it=$cp;
							$gt=false;
							if($n != '/')
								$gt=true;
						}
						else if(($it >= 0) && ($c == '>'))
						{
							$nt .= substr($tl,$it,$cp+1-$it);
							$it=-1;
							$gt=false;
						}
						else if($it < 0)
						{
							if(($maxz > 12) && (($c == '¬') || ($c == ' ')))	// || ($c == ',') || ($c == '-') || ($c == '+') || ($c == ')') || ($c == ']') || ($c == '}') || ($c == ';') || ($c == '.') || ($c == '!') || ($c == '?'))
								$bp=$cp;
							$nt .= $c;
							$cc++;
						}
						else if($gt)
						{
							if($c == '/')
							{
								$tg='';
								$gt=false;
							}
							else if($c == ' ')
								$gt=false;
							else
								$tg .= $c;
						}

						$cp++;
						if(($cp > $cl) || ($cc >= $maxz))
						{
							if($bp > 0)
							{
								$nt=trim(substr($nt,0,$bp));
								$tl=trim(substr($tl,$bp));
							}
							else
								$tl=trim(substr($tl,$cp));
							break;
						}
					}
				}

				while(substr($nt,0,1) == '¬')
					$nt=substr($nt,1);
				while(substr($nt,-1) == '¬')
					$nt=substr($nt,0,-1);
				$nt=trim($nt);

				if(!$wrap)
				{
					$rt .= $nt;
					if(strlen($tg) && ($tg != 'br') && ($tg != 'hr'))
						$rt .= '</'.$tg.'>';
					if(strlen($text) > strlen($rt))
						$rt .= '...';
					break;
				}

				if($plfirst || strlen($rt))
					$rt .= $plstart;
				$rt .= $nt.$plend;

				while(substr($tl,0,1) == '¬')
					$tl=substr($tl,1);
				while(substr($tl,-1) == '¬')
					$tl=substr($tl,0,-1);
				$tl=trim($tl);

				$cl=strlen($tl);
				if($cl)
					$rt .= $line_break.$wrc;
			}
		}
	}

	$rt=strtr($rt, array('¤'=>'&quot;', '¬'=>'&nbsp;'));
//echo('<div style="padding:6px;border:1px solid black;border-radius:4px;margin-bottom:32px;"><b>Rückgabetext:</b><pre>'.fxHtmlEncode($rt).'</pre></div>');
	return $rt;
}

/**
 * extractPost
 * $def (Array)
 * Definition der umzuwandelnden Variablen im Format:
 * array('variablenname|typ' => default)
 * mit Typdefinition:
 * 'a':	array, z.B. Multiselect-Auswahlfelder
 * 'b':	button
 * 'c':	checkbox
 * 'd':	date
 * 'f':	float
 * 'i':	integer
 * 's':	string
 * $tolower (Bool)
 * Falls true werden die Variablennamen nach Lowercase	umgewandelt.
 *
 * @param various $def     - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $tolower - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function extractPost($def, $tolower=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$p = array();

	if(is_array($def) && sizeof($def))
	{
		foreach($def as $vt => $d)
		{
			// Variable initialisieren
			$t  = 'string';
			$vt = trim($vt);
			$sp = strpos($vt, '|');
			if($sp !== false)
			{
				$v = substr($vt, 0, $sp);
				$t = strtolower(substr($vt, $sp+1));
			}
			else
				$v = $vt;

			if($tolower)
				$pk = strtolower($v);
			else
				$pk = $v;

			$tf = substr($t, 0, 1);

			// POST-Wert zuweisen
			if(isset($_POST) && is_array($_POST) && isset($_POST[$v]))
			{
				if($tf == 'b')
					$pv = true;
				else
					$pv = $_POST[$v];
			}
			else if($tf == 'b')
				$pv = false;
			else
				$pv = $d;

			$p[$pk] = $pv;
		}
	}

//fxDebug($p, '$p');
	return $p;
}

/**
 * Check available and necessary PHP extensions
 *
 * @param integer $dbtype - Mandatory parameter: Database type to check for
 *
 * @return Array that contains keys to repesent the status of the extensions
 */
function fxf_checkExtensions($dbtype)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Check if necessary extensions are loaded (!=necessary, #=database)
	$extens=array('!calendar', '!curl', '!iconv', '!mbstring', '!openssl', '!sockets', '!gd2', '#1pgsql', '#2sqlsrv');
	$extdone=array();
	$extstat=array();

	$dbt=0;
	$libok='';
	$cdok='';
	$cdversf=0.0;
	$cdvers='';
	$libnotfound='';
	$cdnotfound='';
	$libmissing='';

	foreach($extens as $ext)
	{
		$fatal=false;
		// Necessary
		if(substr($ext,0,1) == '!') 
		{
			$ext=substr($ext,1);
			$fatal=true;
		}
		// Database
		else if(substr($ext,0,1) == '#')
		{
			$dbt=(int)substr($ext,1,1);
			if($dbtype == $dbt)
				$fatal=true;
			$ext=substr($ext,2);
		}
		if(!extension_loaded($ext))
		{
			$extstat[$ext]=false;
			if($ext == 'gd2')			// If not "gd2", check for "gd"
			{
				if(strlen($libnotfound))
					$libnotfound .= ', ';
				$libnotfound .= '['.$ext.']';
				$ext='gd';
			}
		}
		else
			$extstat[$ext]=true;
		if($extdone[$ext])
			continue;

		$extdone[$ext]=true;
		if(!extension_loaded($ext))
		{
			$extstat[$ext]=false;
			if($fatal)
			{
				if(strlen($libmissing))
					$libmissing .= ', ';
				$libmissing .= '['.$ext.']';
			}
			else
			{
				if(strlen($libnotfound))
					$libnotfound .= ', ';
				$libnotfound .= '['.$ext.']';
			}
		}
		else
		{
			$extstat[$ext]=true;
			if(strlen($libok))
				$libok .= ', ';
			$libok .= '['.$ext.']';
		}
	}

	// Special check for "ChartDirector"
	$cda=fxf_checkChartdir();
	if($cda['cdext'])
	{
		$extstat[$cda['cdlib']]=true;
		$libok .= ', ['.$cda['cdlib'].']';
	}
	else
	{
		$extstat[$cda['cdlib']]=false;
		if(strlen($libnotfound))
			$libnotfound .= ', ';
		$libnotfound .= '['.$cda['cdlib'].']';
	}

	return array('ok'=>$libok, 'notfound'=>$libnotfound, 'missing'=>$libmissing, 'cdext'=>$cda['cdext'], 'cdversf'=>$cda['cdversf'], 'cdvers'=>$cda['cdvers'], 'cdrequire'=>$cda['cdrequire'], 'cdok'=>$cda['cdok'], 'cdnotfound'=>$cda['cdnotfound'], 'extstat'=>$extstat);
}

/**
 * Check if ChartDirector is available
 *
 * @return Array that contains keys to repesent the status of the extensions
 */
function fxf_checkChartdir()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ext='ChartDirector PHP API';

	$cdext=false;
	$cdversf=0.0;
	$cdvers='';
	$cdrequire='';
	$cdkeyfile='';
	$cdkey='';
	$cdok='';
	$cdnotfound='';

	if(extension_loaded($ext))
	{
		$cdext=true;

		$dllVersion=callmethod('getVersion') & 0x7FFF0000;
		if($dllVersion)
		{
			$majorDllVer=($dllVersion >> 24) & 0xFF;
			$minorDllVer=($dllVersion >> 16) & 0xFF;
			$dllVersion=$majorDllVer.'.'.$minorDllVer;
			$cdversf=$dllVersion;
		}
		if($cdversf > 0.0)
			$cdvers=(int)str_replace('.', '', $cdversf);

		if(isset($GLOBALS['_ext_array']['CHARTDIRECTOR']))
			$cdok=$GLOBALS['_ext_array']['CHARTDIRECTOR'];
		else
			$cdnotfound='[phpchartdir.php]';
	}

	if($cdext && strlen($cdok))
	{
		if(!function_exists('cdLoadDLL'))
			$cdrequire=$cdok;

		$cdkf=substr($cdok,0,-3).'ini';
		if(file_exists($cdkf))
		{
			$cdkeyfile=$cdkf;
			$cdka=fxLoad($cdkeyfile, 1);
			if(fxIsArray($cdka))
			{
				$cdkey=trim($cdka[0]);

				$id2=fxf_paramCodeId(2);
				$id=fxf_paramCodeId(0,$cdkey);
				if($id !== $id2)	// Encode and save key
				{
					$cdkey=fxf_paramCodec($cdkey);
					fxSave($cdkeyfile, $cdkey, true);
				}
			}
		}
		else
			$cdkeyfile='';
	}

	return array('cdlib'=>$ext, 'cdext'=>$cdext, 'cdversf'=>$cdversf, 'cdvers'=>$cdvers, 'cdrequire'=>$cdrequire, 'cdkeyfile'=>$cdkeyfile, 'cdkey'=>$cdkey, 'cdok'=>$cdok, 'cdnotfound'=>$cdnotfound);
}

/**
 * ???FUNCTION???
 *
 * @param string $l - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function setFXDebuglevel($l='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$set=true;
	if(strlen($l))
		$GLOBALS['__DGBLVL']=(int)$l;
	else if(fxIsArray($_GET) && isset($_GET['sdbgl']))
		$GLOBALS['__DGBLVL']=(int)$_GET['sdbgl'];
	else if(isset($GLOBALS['sdbgl']))
		$GLOBALS['__DGBLVL']=(int)$GLOBALS['sdbgl'];
	else
	{
		$set=false;
		$GLOBALS['__DGBLVL']=0;
	}

	if($set)
		return $GLOBALS['__DGBLVL'];
	return false;
}

/**
 * Backup or retrieve special global variables
 *
 * @param integer $ntr   - Optional parameter (default = 0): Transaction = Program function: Backup and set special global (>= 0) or retrieve it (<= 0)
 * @param integer $nlang - Optional parameter (default = 0): Language: Backup and set special global (>= 0) or retrieve it (<= 0)
 */
function fxSpecialGlobals($ntr=0, $nlang=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Special global variable: Transaction = Program function
	$ntr=(int)$ntr;
	if($ntr > 0)
	{
		$GLOBALS['_bsg_tr']=$GLOBALS['fxpglobals']['tr'];
		$GLOBALS['fxpglobals']['tr']=$ntr;
		$GLOBALS['tr']=$ntr;
	}
	else if(isset($GLOBALS['_bsg_tr']))
	{
		$GLOBALS['fxpglobals']['tr']=$GLOBALS['_bsg_tr'];
		$GLOBALS['tr']=$GLOBALS['_bsg_tr'];
	}
	else
		$GLOBALS['_bsg_tr']=$GLOBALS['fxpglobals']['tr'];

	// Special global variable: Language
	$nlang=(int)$nlang;
	if($nlang > 0)
	{
		$GLOBALS['_bsg_lang']=$GLOBALS['fxpglobals']['lang'];
		$GLOBALS['fxpglobals']['lang']=$nlang;
	}
	else if(isset($GLOBALS['_bsg_lang']))
		$GLOBALS['fxpglobals']['lang']=$GLOBALS['_bsg_lang'];
	else
		$GLOBALS['_bsg_lang']=$GLOBALS['fxpglobals']['lang'];
}

/**
 * Get valid image path for an image filename
 *
 * @param string  $ifn - Mandatory parameter:               Image filename
 * @param integer $cid - Optional parameter (default = 0):  Client id (if 0, current client id is used)
 *
 * @return Image path
 */
function fxGetImagePath($ifn, $cid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ifn=trim($ifn);
	$ipn='';

	if(strlen($ifn))
	{
		$cid=(int)$cid;
		if(!$cid)
			$cid=$GLOBALS['fxpglobals']['client'];

		$ifn=basename($ifn);
		$spp=strrpos($ifn,'.');
		if($spp === false)
			$ifn .= '.png';
		$isn=$ifn;
		if(substr($isn,-4,1) == '.')	// Name without suffix
			$isn=substr($isn,0,-3);
		if(substr($isn,-1) != '.')
			$isn .= '.';

		// Check paths and suffixes
		$pa=array($GLOBALS['datpath'].$cid.'/', $GLOBALS['datpath'].'0/', $GLOBALS['gfxpath']);
		$sa=array('png', 'jpg', 'gif');

		foreach($pa as $p)
		{
			$cpn=$p.$ifn;
			if(file_exists($cpn))
			{
				$ipn=$cpn;
				break;
			}
			foreach($sa as $s)
			{
				$cpn=$p.$isn.$s;
				if(file_exists($cpn))
				{
					$ipn=$cpn;
					break 2;
				}
			}
		}
	}
//echo('<b>fxGetImagePath:</b> '.$ifn.' &rarr; '.$ipn.'<br />');

	return $ipn;
}
?>