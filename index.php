<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : index.php                                                    //
// Version     : 21.1                                                         //
// Begin       : 2020-07-21                                                   //
// Last Change : 2021-03-29                                                   //
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
 * Main program to create the html startup layout from a template.
 * The html javascript then calls via AJAX the main control program.
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// Special debug mode
////////////////////////////////////////////////////////////////////////////////
$GLOBALS['__debug']=array('__DBG'=>0);
// Debug this file? (Uncomment the next line)
//$GLOBALS['__debug'][__FILE__]=3;
// Display the debug selector (false or true)
$GLOBALS['__debug']['debug_selector']=true;

// Send these additional headers
$__headers=array(
	'Last-Modified: '.gmdate('D, d M Y H:i:s').' GMT',	// current date, because it is created differently ever time dynamically
	'Cache-Control: no-cache, must-revalidate',  		// HTTP/1.1
	'Pragma: no-cache'                         			// HTTP/1.0
);

// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

// Load HTML template
$__htf='index_template.html';
$html=fxLoad($__htf);
if(!$html || !strlen($html))
	die('<h1 style="padding:8px;color:red;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary HTML template &quot;'.$__htf.'&quot; not found!</h1>');

ob_start();

// Get start texts in current language
$lang=fxGetLanguage(0, false);
$stla=fxGetLanguageTexts('start',true,false,$lang,false);
fxDebug($stla,'$stla: fxGetLanguageTexts(\'start\')',2);

// FXP javascript file (main javascript function collection)
$jscfxp=$GLOBALS['jscpath'].'fxp.js';

// EXT javascript file (extended javascript function collection)
$jscext='';
$jscextfile=$GLOBALS['jscpath'].'ext.js';
if(file_exists($jscextfile))
	$jscext=' <script type="text/javascript" src="'.$jscextfile.'?'.filemtime($jscextfile).'"></script>';

// Replace certain placeholders in the HTML template
$_tra=array(
	'TITLE'				=> 'fx-project '.$v_type.' V'.$v_fxp,
	'FXPTYPE'			=> $v_type,
	'FXPTCOL'			=> $v_tcol,
	'VERSION'			=> $v_fxp,
	'CHARSET'			=> FXP_CHARSET,
	'DATE'				=> substr($GLOBALS['datetime'],0,4).'-'.substr($GLOBALS['datetime'],4,2).'-'.substr($GLOBALS['datetime'],6,2).'T00:00:00+00:00',
	'LANGUAGE'			=> $lang,
	'LANGUAGE2'			=> substr('0'.$lang,-2),
	'CSS_DEFAULT'		=> $GLOBALS['cssfile'],
	'FAVICON'			=> fxGetFavicon(),
	'SEO_DESCRIPTION'	=> 'FeRox Management Consulting GmbH & Co. KG: Open Source Project Management Software fx-project V'.$v_fxp,
	'SEO_URL'			=> $GLOBALS['__server_array']['urls'],
	'SEO_IMAGE'			=> $GLOBALS['__server_array']['url'].'GFX/fxp_image.png',
	'FXPLOGO'			=> fxGetLogo('big'),
	'FXPLOGOBAR'		=> fxGetLogo('small'),
	'SCDISPLAY'			=> ((isset($GLOBALS['spppath']) && strlen($GLOBALS['spppath']) && file_exists($GLOBALS['spppath'].'htm2png.exe'))?'block':'none'),
	'LTS'				=> $GLOBALS['lts'],
	'IDATA'				=> $GLOBALS['locseskey'].'|'.$GLOBALS['lts'].'|'.$lang.'|'.$GLOBALS['fxpglobals']['design'].'|'.$GLOBALS['fxpglobals']['zoom'],
	'SESS_UPN'			=> ini_get('session.upload_progress.name'),
	'JSC_PREFIX'		=> fxf_jsFunctionPrefix(),
	'JSC_FXP'			=> $jscfxp.'?'.filemtime($jscfxp),
	'JSC_EXT'			=> $jscext
);
// ...as well as all subpaths
$_tra['MAIN_PATH']=$GLOBALS['mainpath'];
if(fxIsArray($GLOBALS['_subpath_array']))
{
	foreach($GLOBALS['_subpath_array'] as $spname => $sppath)
		$_tra[$spname.'_PATH']=$sppath;
}

// ...as well as start texts in current language
if(fxIsArray($stla))
{
	foreach($stla as $variable => $text)
	{
		$fc=substr($text,0,1);
		if(($fc == '^') || ($fc == '~'))
			$text=substr($text,1);
		$_tra[$variable]=$text;
	}
}
// ...as well as all javascript separators
foreach($GLOBALS['_divstr'] as $jdsc => $jdss)
	$_tra['divstr_'.$jdsc]=$jdss;
// ...as well as the current filter status (0=off + 1=on)
$_tra['fltstat']=(int)$GLOBALS['fxpglobals']['setfilter'];
// ...as well as the debug selector?
if($GLOBALS['__debug']['debug_selector'])
{
	$_tra['debugselector']  = '<div id="dbgmode" class="ticon" tooltip="'.$_tra['debugtxt'].'" style="position:absolute;left:56px;top:2px;cursor:pointer;">';
	$_tra['debugselector'] .= '<div style="float:left;"><img src="'.$GLOBALS['gfxpath'].'dbg_b_80x80.png" style="width:32px;height:32px;"><img src="'.$GLOBALS['gfxpath'].'dbg_w_80x80.png" style="position:absolute;left:2px;top:2px;width:32px;height:32px;"></div>';
	$_tra['debugselector'] .= '<div id="dbgmodedbg" style="float:left;margin-left:4px;margin-top:16px;font-size:16px;font-weight:bold;color:#000;">'.$GLOBALS['__debug']['__DBG'].'</div>';
	$_tra['debugselector'] .= '</div>';
}

fxDebug($_tra,'$_tra',2);

foreach($_tra as $placeholder => $text)
	$html=str_ireplace('<!--#'.strtoupper($placeholder).'#-->', $text, $html);

// Work area output
$output=ob_get_contents();
ob_end_clean();
$html=str_ireplace('<!--#WORKAREA#-->', $output, $html);

// Display adjusted HTML
echo($html);
?>