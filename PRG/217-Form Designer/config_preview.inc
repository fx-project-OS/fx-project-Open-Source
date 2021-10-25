<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : config_preview.inc                                           //
// Version     : 21.2                                                         //
// Begin       : 2020-10-15                                                   //
// Last Change : 2021-03-30                                                   //
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
 * PF 217: Form Designer - Preview
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(isset($_GET) && isset($_GET['pf']) && strlen($_GET['pf']))
{
	$filename=urldecode($GLOBALS['tmppath'].'~'.trim($_GET['pf']).'.pdf');
	if(file_exists($filename))
	{
		// Disable output compression, or else the filesize doesn't match
		ini_set('zlib.output_compression', 'Off');

		$len = filesize($filename);

		@header('Expires: 0');
		@header('Cache-Control: store, public, must-revalidate');
		@header('Pragma: hack'); // Hack für IE
		@header('Content-Length: '.$len);
		@header('Content-Transfer-Encoding: binary');

		@header('Content-Type: application/pdf');
		@header('Content-Disposition: inline; filename="test-'.$_GET['config'].'-'.time().'.pdf"');

		@readfile($filename);
	}
	else
		echo('<b>File not found!</b>');

	die;
}

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

setForm(true);
fxSession('t','l');

$nltr = array("\r\n"=>'<:nl:>', "\r"=>'<:nl:>', "\n"=>'<:nl:>');
$_preview_config = explode('<:nl:>', strtr($_POST['Text_varLang'], $nltr));

$lng = 0;
if(!$lng && fxIsArray($_POST) && isset($_POST['lng']))
	$lng = (int)$_POST['lng'];
if(!$lng)
	$lng = $GLOBALS['fxpglobals']['lang'];

$incfn='';

// Deutsch
if($lng == 1)
{
	$_preview_subst['name']		= 'Mustermann, Max';
	$_preview_subst['project']	= '50 MP';
	$_preview_subst['timespan']	= '01.07.2013 - 31.08.2013';
}
// English
else
{
	$_preview_subst['name']		= 'Doe, John';
	$_preview_subst['project']	= '50 MP';
	$_preview_subst['timespan']	= '07/01/2013 - 08/31/2013';
}

if($_POST['ktyp'] == 2018)		// Email
{
	$lorem_ipsum  = 'Lorem ipsum dolor sit amet, consectetuer sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.<br><br>';
	$lorem_ipsum .=	'Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.<br><br>';
	$lorem_ipsum .= 'Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.';

	// We need person functions
	$GLOBALS['__includer']['tools_pers']=true;
	require('includer.inc');

	$preview=send_mail($GLOBALS['fxpglobals']['person'], 'Test', '<b>Lorem ipsum</b><br><br><br>'.$lorem_ipsum, 0, 0, false, true);

	echo($preview);
}
else if($_POST['ktyp'] == 2020)	// Grafikdiagramme
{
	$_preview_cal=array();

	// Deutsch
	$_preview_cal[0][0]['text']='&nbsp;<b>Projektstatus</b>:&nbsp;';
	$_preview_cal[0][0]['color']='ffffff';
	$_preview_cal[0][0]['typ']='All';

	$_preview_cal[0][1]['text']='&nbsp;Alle Projekte&nbsp;';
	$_preview_cal[0][1]['color']='f8f8f8';
	$_preview_cal[0][1]['colspan']=3;
	$_preview_cal[0][1]['typ']='All';

	$_preview_cal[0][4]['text']='August 2013';
	$_preview_cal[0][4]['colspan']=26;
	$_preview_cal[0][4]['class']='mn1';
	$_preview_cal[0][4]['font']='color="#787878"';


	$_preview_cal[1][0]['text']='&nbsp;<b>Zeitspanne</b>:&nbsp;';
	$_preview_cal[1][0]['color']='ffffff';
	$_preview_cal[1][0]['typ']='All';

	$_preview_cal[1][1]['text']='&nbsp;05.08.2013&nbsp;-&nbsp;30.08.2013&nbsp;';
	$_preview_cal[1][1]['color']='f8f8f8';
	$_preview_cal[1][1]['colspan']=3;
	$_preview_cal[1][1]['typ']='All';

	$_preview_cal[1][4]['text']='KW 32';
	$_preview_cal[1][4]['colspan']=7;
	$_preview_cal[1][4]['class']='mn0';
	$_preview_cal[1][4]['font']='color="#787878"';

	$_preview_cal[1][11]['text']='KW 33';
	$_preview_cal[1][11]['colspan']=7;
	$_preview_cal[1][11]['class']='mn1';
	$_preview_cal[1][11]['font']='color="#787878"';

	$_preview_cal[1][18]['text']='KW 34';
	$_preview_cal[1][18]['colspan']=7;
	$_preview_cal[1][18]['class']='mn0';
	$_preview_cal[1][18]['font']='color="#787878"';

	$_preview_cal[1][25]['text']='KW 35';
	$_preview_cal[1][25]['colspan']=5;
	$_preview_cal[1][25]['class']='mn1';
	$_preview_cal[1][25]['font']='color="#787878"';

	$_preview_cal[2][0]['text']='&nbsp;<b>Projekt-Name</b>&nbsp;';
	$_preview_cal[2][0]['color']='dddddd';
	$_preview_cal[2][0]['typ']='All';

	$_preview_cal[2][1]['text']='&nbsp;<b>Projekt-Dauer</b>&nbsp;';
	$_preview_cal[2][1]['color']='dddddd';
	$_preview_cal[2][1]['typ']='All';

	$_preview_cal[2][2]['text']='<b>Info</b>';
	$_preview_cal[2][2]['color']='dddddd';
	$_preview_cal[2][2]['align']='center';
	$_preview_cal[2][2]['colspan']=2;
	$_preview_cal[2][2]['typ']='All';

	$_preview_cal[2][4]['text']='<div style="width:16px;overflow:hidden;">05</div>';
	$_preview_cal[2][4]['typ']='All';
	$_preview_cal[2][4]['class']='twd';
	$_preview_cal[2][4]['font']='color="#787878"';

	$_preview_cal[2][5]['text']='<div style="width:16px;overflow:hidden;">06</div>';
	$_preview_cal[2][5]['typ']='All';
	$_preview_cal[2][5]['class']='twd';
	$_preview_cal[2][5]['font']='color="#787878"';

	$_preview_cal[2][6]['text']='<div style="width:16px;overflow:hidden;">07</div>';
	$_preview_cal[2][6]['typ']='All';
	$_preview_cal[2][6]['class']='twd';
	$_preview_cal[2][6]['font']='color="#787878"';

	$_preview_cal[2][7]['text']='<div style="width:16px;overflow:hidden;">08</div>';
	$_preview_cal[2][7]['typ']='All';
	$_preview_cal[2][7]['class']='twd';
	$_preview_cal[2][7]['font']='color="#787878"';

	$_preview_cal[2][8]['text']='<div style="width:16px;overflow:hidden;">09</div>';
	$_preview_cal[2][8]['typ']='All';
	$_preview_cal[2][8]['class']='twd';
	$_preview_cal[2][8]['font']='color="#787878"';

	$_preview_cal[2][9]['text']='<div style="width:16px;overflow:hidden;">10</div>';
	$_preview_cal[2][9]['typ']='All';
	$_preview_cal[2][9]['class']='twe';
	$_preview_cal[2][9]['font']='color="#787878"';

	$_preview_cal[2][10]['text']='<div style="width:16px;overflow:hidden;">11</div>';
	$_preview_cal[2][10]['typ']='All';
	$_preview_cal[2][10]['class']='twe';
	$_preview_cal[2][10]['font']='color="#787878"';

	$_preview_cal[2][11]['text']='<div style="width:16px;overflow:hidden;">12</div>';
	$_preview_cal[2][11]['typ']='All';
	$_preview_cal[2][11]['class']='twd';
	$_preview_cal[2][11]['font']='color="#787878"';

	$_preview_cal[2][12]['text']='<div style="width:16px;overflow:hidden;">13</div>';
	$_preview_cal[2][12]['typ']='All';
	$_preview_cal[2][12]['class']='twd';
	$_preview_cal[2][12]['font']='color="#787878"';

	$_preview_cal[2][13]['text']='<div style="width:16px;overflow:hidden;">14</div>';
	$_preview_cal[2][13]['typ']='All';
	$_preview_cal[2][13]['class']='twd';
	$_preview_cal[2][13]['font']='color="#787878"';

	$_preview_cal[2][14]['text']='<div style="width:16px;overflow:hidden;">15</div>';
	$_preview_cal[2][14]['typ']='All';
	$_preview_cal[2][14]['class']='twd';
	$_preview_cal[2][14]['font']='color="#787878"';

	$_preview_cal[2][15]['text']='<div style="width:16px;overflow:hidden;">16</div>';
	$_preview_cal[2][15]['typ']='All';
	$_preview_cal[2][15]['class']='twd';
	$_preview_cal[2][15]['font']='color="#787878"';

	$_preview_cal[2][16]['text']='<div style="width:16px;overflow:hidden;">17</div>';
	$_preview_cal[2][16]['typ']='All';
	$_preview_cal[2][16]['class']='twe';
	$_preview_cal[2][16]['font']='color="#787878"';

	$_preview_cal[2][17]['text']='<div style="width:16px;overflow:hidden;">18</div>';
	$_preview_cal[2][17]['typ']='All';
	$_preview_cal[2][17]['class']='twe';
	$_preview_cal[2][17]['font']='color="#787878"';

	$_preview_cal[2][18]['text']='<div style="width:16px;overflow:hidden;">19</div>';
	$_preview_cal[2][18]['typ']='All';
	$_preview_cal[2][18]['class']='twd';
	$_preview_cal[2][18]['font']='color="#787878"';

	$_preview_cal[2][19]['text']='<div style="width:16px;overflow:hidden;">20</div>';
	$_preview_cal[2][19]['typ']='All';
	$_preview_cal[2][19]['class']='twd';
	$_preview_cal[2][19]['font']='color="#787878"';

	$_preview_cal[2][20]['text']='<div style="width:16px;overflow:hidden;">21</div>';
	$_preview_cal[2][20]['typ']='All';
	$_preview_cal[2][20]['class']='twd';
	$_preview_cal[2][20]['font']='color="#787878"';

	$_preview_cal[2][21]['text']='<div style="width:16px;overflow:hidden;">22</div>';
	$_preview_cal[2][21]['typ']='All';
	$_preview_cal[2][21]['class']='twd';
	$_preview_cal[2][21]['font']='color="#787878"';

	$_preview_cal[2][22]['text']='<div style="width:16px;overflow:hidden;">23</div>';
	$_preview_cal[2][22]['typ']='All';
	$_preview_cal[2][22]['class']='twd';
	$_preview_cal[2][22]['font']='color="#787878"';

	$_preview_cal[2][23]['text']='<div style="width:16px;overflow:hidden;">24</div>';
	$_preview_cal[2][23]['typ']='All';
	$_preview_cal[2][23]['class']='twe';
	$_preview_cal[2][23]['font']='color="#787878"';

	$_preview_cal[2][24]['text']='<div style="width:16px;overflow:hidden;">25</div>';
	$_preview_cal[2][24]['typ']='All';
	$_preview_cal[2][24]['class']='twe';
	$_preview_cal[2][24]['font']='color="#787878"';

	$_preview_cal[2][25]['text']='<div style="width:16px;overflow:hidden;">26</div>';
	$_preview_cal[2][25]['typ']='All';
	$_preview_cal[2][25]['class']='twd';
	$_preview_cal[2][25]['font']='color="#787878"';

	$_preview_cal[2][26]['text']='<div style="width:16px;overflow:hidden;">27</div>';
	$_preview_cal[2][26]['typ']='All';
	$_preview_cal[2][26]['class']='twd';
	$_preview_cal[2][26]['font']='color="#787878"';

	$_preview_cal[2][27]['text']='<div style="width:16px;overflow:hidden;">28</div>';
	$_preview_cal[2][27]['typ']='All';
	$_preview_cal[2][27]['class']='twd';
	$_preview_cal[2][27]['font']='color="#787878"';

	$_preview_cal[2][28]['text']='<div style="width:16px;overflow:hidden;">29</div>';
	$_preview_cal[2][28]['typ']='All';
	$_preview_cal[2][28]['class']='twd';
	$_preview_cal[2][28]['font']='color="#787878"';

	$_preview_cal[2][29]['text']='<div style="width:16px;overflow:hidden;">30</div>';
	$_preview_cal[2][29]['typ']='All';
	$_preview_cal[2][29]['class']='twd';
	$_preview_cal[2][29]['font']='color="#787878"';


	$_preview_cal[3][0]['text']='<input type="hidden" id="disp23193" name="disp23193" fxform="_fxform_selmask_1" value="_cl"><input type="hidden" id="pr[3]" name="pr[3]" fxform="_fxform_selmask_1" value="23193"><input type="hidden" id="vn[3]" name="vn[3]" fxform="_fxform_selmask_1" value="0050"><img id="its3" src="'.$GLOBALS['gfxpath'].'fl_cl.png" hspace="2" style="cursor:pointer;" '.fxf_jsFunction('toggleSubs', 3, '0050').'>&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt59.png" align="middle">&nbsp;<font class="darkgrey">50</font>&nbsp;<font class="grey">MP</font>&nbsp;';
	$_preview_cal[3][0]['class']='yat1';
	$_preview_cal[3][0]['disp']='(leer)';
	$_preview_cal[3][0]['typ']='All';
	$_preview_cal[3][0]['sk']=4;
	$_preview_cal[3][0]['ek']=29;

	$_preview_cal[3][1]['text']='<a href="#pj1">&nbsp;<i class="grey">(05.08.2013&nbsp;-&nbsp;30.08.2013)</i>&nbsp;<br></a>';
	$_preview_cal[3][1]['class']='yat1';
	$_preview_cal[3][1]['align']='center';
	$_preview_cal[3][1]['typ']='All';

	$_preview_cal[3][2]['align']='center';
	$_preview_cal[3][2]['tooltip_js']='Das geplante Projektende wurde bereits überschritten. Die Aufgabe wurde noch nicht begonnen!';
	$_preview_cal[3][2]['text']='<img src="'.$GLOBALS['gfxpath'].'c_msg.png" style="margin:2px;margin-top:2px;">';
	$_preview_cal[3][2]['typ']='All';
	$_preview_cal[3][2]['class']='yat1';

	$_preview_cal[3][3]['text']='<span style="padding:0 2px;"><img id="report3" name="report3" class="fxfil" src=""></span>';
	$_preview_cal[3][3]['align']='right';
	$_preview_cal[3][3]['class']='yat1';
	$_preview_cal[3][3]['typ']='All';

	$_preview_cal[3][4]['class']='krz1';
	$_preview_cal[3][4]['colspan']=26;
	$_preview_cal[3][4]['background']='pspr_sb';
	$_preview_cal[3][4]['ltextz']='<div style="position:relative;left:4px;top:15px;">';
	$_preview_cal[3][4]['rtextz']='</div>';
	$_preview_cal[3][4]['text']='MP';
	$_preview_cal[3][4]['pkl']='psprl';
	$_preview_cal[3][4]['ltext']='<img src="'.$GLOBALS['gfxpath'].'psprl.png" align="left" valign="top">';
	$_preview_cal[3][4]['pkr']='psprr';
	$_preview_cal[3][4]['rtext']='<img src="'.$GLOBALS['gfxpath'].'psprr.png" align="right" valign="top">';
	$_preview_cal[3][4]['align']='left" valign="top';
	$_preview_cal[3][4]['tooltip_js']='<div class=rhd>50&nbsp;-&nbsp;MP</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Soll-Zeitraum:</b></td><td nowrap>05.08.2013&nbsp;-&nbsp;30.08.2013</td></tr><tr><td nowrap><b>Projektstatus:</b></td><td nowrap>geplant</td></tr></table>';
	$_preview_cal[3][4]['anchor']=true;
	$_preview_cal[3][4]['status']=297;
	$_preview_cal[3][4]['mode']='010';

	$_preview_cal[4][0]['text']='<input type="hidden" id="disp23194" name="disp23194" fxform="_fxform_selmask_1" value="_cl"><input type="hidden" id="pr[4]" name="pr[4]" fxform="_fxform_selmask_1" value="23194"><input type="hidden" id="vn[4]" name="vn[4]" fxform="_fxform_selmask_1" value="0050.0001"><img id="its4" src="'.$GLOBALS['gfxpath'].'fl_cl.png" hspace="2" style="cursor:pointer;" '.fxf_jsFunction('toggleSubs', 4, '0050.0001').'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt60.png" align="middle">&nbsp;<font class="darkgrey">50.1</font>&nbsp;<font class="grey">TP1</font>&nbsp;';
	$_preview_cal[4][0]['class']='yat0';
	$_preview_cal[4][0]['disp']='(leer)';
	$_preview_cal[4][0]['typ']='All';
	$_preview_cal[4][0]['sk']=4;
	$_preview_cal[4][0]['ek']=22;

	$_preview_cal[4][1]['text']='<a href="#pj2">&nbsp;<i class="grey">(05.08.2013&nbsp;-&nbsp;23.08.2013)</i>&nbsp;<br></a>';
	$_preview_cal[4][1]['class']='yat0';
	$_preview_cal[4][1]['align']='center';
	$_preview_cal[4][1]['typ']='All';

	$_preview_cal[4][2]['align']='center';
	$_preview_cal[4][2]['tooltip_js']='Das geplante Projektende wurde bereits überschritten. Die Aufgabe wurde noch nicht begonnen!';
	$_preview_cal[4][2]['text']='<img src="'.$GLOBALS['gfxpath'].'c_msg.png" style="margin:2px;margin-top:2px;">';
	$_preview_cal[4][2]['typ']='All';
	$_preview_cal[4][2]['class']='yat0';

	$_preview_cal[4][3]['text']='<span style="padding:0 2px;"><img id="report4" name="report4" class="fxfil" src=""></span>';
	$_preview_cal[4][3]['align']='right';
	$_preview_cal[4][3]['class']='yat0';
	$_preview_cal[4][3]['typ']='All';

	$_preview_cal[4][4]['class']='krz0';
	$_preview_cal[4][4]['colspan']=19;
	$_preview_cal[4][4]['background']='pspr_sb';
	$_preview_cal[4][4]['ltextz']='<div style="position:relative;left:4px;top:15px;">';
	$_preview_cal[4][4]['rtextz']='</div>';
	$_preview_cal[4][4]['text']='TP1';
	$_preview_cal[4][4]['pkl']='psprl';
	$_preview_cal[4][4]['ltext']='<img src="'.$GLOBALS['gfxpath'].'psprl.png" align="left" valign="top">';
	$_preview_cal[4][4]['pkr']='psprr';
	$_preview_cal[4][4]['rtext']='<img src="'.$GLOBALS['gfxpath'].'psprr.png" align="right" valign="top">';
	$_preview_cal[4][4]['align']='left" valign="top';
	$_preview_cal[4][4]['tooltip_js']='<div class=rhd>50.1&nbsp;-&nbsp;TP1</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Soll-Zeitraum:</b></td><td nowrap>05.08.2013&nbsp;-&nbsp;23.08.2013</td></tr><tr><td nowrap><b>Projektstatus:</b></td><td nowrap>geplant</td></tr></table>';
	$_preview_cal[4][4]['link']='?tr=23&aktion=1&gproject=23194';
	$_preview_cal[4][4]['anchor']=true;
	$_preview_cal[4][4]['status']=297;
	$_preview_cal[4][4]['mode']='010';

	$_preview_cal[4][23]['colspan']=7;
	$_preview_cal[4][23]['color']='d4d4cf';


	$_preview_cal[5][0]['text']='<input type="hidden" id="disp23195" name="disp23195" fxform="_fxform_selmask_1" value="_"><input type="hidden" id="pr[5]" name="pr[5]" fxform="_fxform_selmask_1" value="23195"><input type="hidden" id="vn[5]" name="vn[5]" fxform="_fxform_selmask_1" value="0050.0001.0001"><img src="'.$GLOBALS['gfxpath'].'dmy.png" width="13" height="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt61.png" align="middle">&nbsp;<font class="black">50.1.1</font>&nbsp;<font class="darkgrey">A1-1</font>&nbsp;';
	$_preview_cal[5][0]['class']='yat1';
	$_preview_cal[5][0]['disp']='(leer)';
	$_preview_cal[5][0]['typ']='All';
	$_preview_cal[5][0]['sk']=4;
	$_preview_cal[5][0]['ek']=15;

	$_preview_cal[5][1]['text']='<a href="#pj3">&nbsp;<i class="grey">(05.08.2013&nbsp;-&nbsp;16.08.2013)</i>&nbsp;<br></a>';
	$_preview_cal[5][1]['class']='yat1';
	$_preview_cal[5][1]['align']='center';
	$_preview_cal[5][1]['typ']='All';

	$_preview_cal[5][2]['align']='center';
	$_preview_cal[5][2]['tooltip_js']='Das geplante Projektende wurde bereits überschritten. Die Aufgabe wurde noch nicht begonnen!';
	$_preview_cal[5][2]['text']='<img src="'.$GLOBALS['gfxpath'].'c_msg.png" style="margin:2px;margin-top:2px;">';
	$_preview_cal[5][2]['typ']='All';
	$_preview_cal[5][2]['class']='yat1';

	$_preview_cal[5][3]['text']='<span style="padding:0 2px;"><img id="report5" name="report5" class="fxfil" src=""></span>';
	$_preview_cal[5][3]['align']='right';
	$_preview_cal[5][3]['class']='yat1';
	$_preview_cal[5][3]['typ']='All';

	$_preview_cal[5][4]['class']='krz1';
	$_preview_cal[5][4]['colspan']=12;
	$_preview_cal[5][4]['background']='psau_sb';
	$_preview_cal[5][4]['ltextz']='<div style="position:relative;left:4px;top:15px;">';
	$_preview_cal[5][4]['rtextz']='</div>';
	$_preview_cal[5][4]['text']='A1-1';
	$_preview_cal[5][4]['pkl']='psau297l';
	$_preview_cal[5][4]['ltext']='<img src="'.$GLOBALS['gfxpath'].'psau297l.png" align="left" valign="top">';
	$_preview_cal[5][4]['pkr']='psau297r';
	$_preview_cal[5][4]['rtext']='<img src="'.$GLOBALS['gfxpath'].'psau297r.png" align="right" valign="top">';
	$_preview_cal[5][4]['align']='left" valign="top';
	$_preview_cal[5][4]['tooltip_js']='<div class=rhd>50.1.1&nbsp;-&nbsp;A1-1</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Soll-Zeitraum:</b></td><td nowrap>05.08.2013&nbsp;-&nbsp;16.08.2013</td></tr><tr><td nowrap><b>Projektstatus:</b></td><td nowrap>geplant</td></tr></table>';
	$_preview_cal[5][4]['link']='?tr=23&aktion=1&gproject=23195';
	$_preview_cal[5][4]['anchor']=true;
	$_preview_cal[5][4]['status']=297;
	$_preview_cal[5][4]['mode']='010';

	$_preview_cal[5][16]['colspan']=14;
	$_preview_cal[5][16]['color']='d4d4cf';


	$_preview_cal[6][0]['text']='<input type="hidden" id="disp23196" name="disp23196" fxform="_fxform_selmask_1" value="_"><input type="hidden" id="pr[6]" name="pr[6]" fxform="_fxform_selmask_1" value="23196"><input type="hidden" id="vn[6]" name="vn[6]" fxform="_fxform_selmask_1" value="0050.0001.0002"><img src="'.$GLOBALS['gfxpath'].'dmy.png" width="13" height="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt61.png" align="middle">&nbsp;<font class="black">50.1.2</font>&nbsp;<font class="darkgrey">A1-2</font>&nbsp;';
	$_preview_cal[6][0]['class']='yat0';
	$_preview_cal[6][0]['disp']='(leer)';
	$_preview_cal[6][0]['typ']='All';
	$_preview_cal[6][0]['sk']=11;
	$_preview_cal[6][0]['mst']=array(0=>array(
		'name'=>'Meilenstein: Ende Abschnitt 1 (23.08.2013)',
		'col'=>22));
	$_preview_cal[6][0]['ek']=22;

	$_preview_cal[6][1]['text']='<a href="#pj4">&nbsp;<i class="grey">(12.08.2013&nbsp;-&nbsp;23.08.2013)</i>&nbsp;<br></a>';
	$_preview_cal[6][1]['class']='yat0';
	$_preview_cal[6][1]['align']='center';
	$_preview_cal[6][1]['typ']='All';

	$_preview_cal[6][2]['align']='center';
	$_preview_cal[6][2]['tooltip_js']='Das geplante Projektende wurde bereits überschritten. Die Aufgabe wurde noch nicht begonnen!';
	$_preview_cal[6][2]['text']='<img src="'.$GLOBALS['gfxpath'].'c_msg.png" style="margin:2px;margin-top:2px;">';
	$_preview_cal[6][2]['typ']='All';
	$_preview_cal[6][2]['class']='yat0';

	$_preview_cal[6][3]['text']='<span style="padding:0 2px;"><img id="report6" name="report6" class="fxfil" src=""></span>';
	$_preview_cal[6][3]['align']='right';
	$_preview_cal[6][3]['class']='yat0';
	$_preview_cal[6][3]['typ']='All';

	$_preview_cal[6][4]['colspan']=7;
	$_preview_cal[6][4]['color']='d4d4cf';

	$_preview_cal[6][11]['class']='krz0';
	$_preview_cal[6][11]['colspan']=12;
	$_preview_cal[6][11]['background']='psau_sb';
	$_preview_cal[6][11]['ltextz']='<div style="position:relative; left:0; top:0; text-align:right;"><img src="'.$GLOBALS['gfxpath'].'psml.png" border="0" align="right" valign="top" hspace="0" title="Meilenstein: Ende Abschnitt 1 (23.08.2013)" style="position:absolute; margin-top:16px; margin-left:-6px;"></div><div style="position:relative;left:4px;top:15px;">';
	$_preview_cal[6][11]['rtextz']='</div>';
	$_preview_cal[6][11]['text']='A1-2';
	$_preview_cal[6][11]['pkl']='psau297l';
	$_preview_cal[6][11]['ltext']='<img src="'.$GLOBALS['gfxpath'].'psau297l.png" align="left" valign="top">';
	$_preview_cal[6][11]['pkr']='psau297r';
	$_preview_cal[6][11]['rtext']='<img src="'.$GLOBALS['gfxpath'].'psau297r.png" align="right" valign="top">';
	$_preview_cal[6][11]['align']='left" valign="top';
	$_preview_cal[6][11]['tooltip_js']='<div class=rhd>50.1.2&nbsp;-&nbsp;A1-2</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Soll-Zeitraum:</b></td><td nowrap>12.08.2013&nbsp;-&nbsp;23.08.2013</td></tr><tr><td nowrap><b>Meilenstein:</b></td><td nowrap>Ende Abschnitt 1 (23.08.2013)<br></td></tr><tr><td nowrap><b>Projektstatus:</b></td><td nowrap>geplant</td></tr></table>';
	$_preview_cal[6][11]['link']='?tr=23&aktion=1&gproject=23196';
	$_preview_cal[6][11]['anchor']=true;
	$_preview_cal[6][11]['status']=297;
	$_preview_cal[6][11]['mode']='010';

	$_preview_cal[6][23]['colspan']=7;
	$_preview_cal[6][23]['color']='d4d4cf';


	$_preview_cal[7][0]['text']='<input type="hidden" id="disp23197" name="disp23197" fxform="_fxform_selmask_1" value="_cl"><input type="hidden" id="pr[7]" name="pr[7]" fxform="_fxform_selmask_1" value="23197"><input type="hidden" id="vn[7]" name="vn[7]" fxform="_fxform_selmask_1" value="0050.0002"><img id="its7" src="'.$GLOBALS['gfxpath'].'fl_cl.png" hspace="2" style="cursor:pointer;" '.fxf_jsFunction('toggleSubs', 7, '0050.0002').'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt60.png" align="middle">&nbsp;<font class="darkgrey">50.2</font>&nbsp;<font class="grey">TP2</font>&nbsp;';
	$_preview_cal[7][0]['class']='yat1';
	$_preview_cal[7][0]['disp']='(leer)';
	$_preview_cal[7][0]['typ']='All';
	$_preview_cal[7][0]['sk']=25;
	$_preview_cal[7][0]['ek']=29;

	$_preview_cal[7][1]['text']='<a href="#pj5">&nbsp;<i class="grey">(26.08.2013&nbsp;-&nbsp;30.08.2013)</i>&nbsp;<br></a>';
	$_preview_cal[7][1]['class']='yat1';
	$_preview_cal[7][1]['align']='center';
	$_preview_cal[7][1]['typ']='All';

	$_preview_cal[7][2]['align']='center';
	$_preview_cal[7][2]['tooltip_js']='Das geplante Projektende wurde bereits überschritten. Die Aufgabe wurde noch nicht begonnen!';
	$_preview_cal[7][2]['text']='<img src="'.$GLOBALS['gfxpath'].'c_msg.png" style="margin:2px;margin-top:2px;">';
	$_preview_cal[7][2]['typ']='All';
	$_preview_cal[7][2]['class']='yat1';

	$_preview_cal[7][3]['text']='<span style="padding:0 2px;"><img id="report7" name="report7" class="fxfil" src=""></span>';
	$_preview_cal[7][3]['align']='right';
	$_preview_cal[7][3]['class']='yat1';
	$_preview_cal[7][3]['typ']='All';

	$_preview_cal[7][4]['colspan']=21;
	$_preview_cal[7][4]['color']='d4d4cf';

	$_preview_cal[7][25]['class']='krz1';
	$_preview_cal[7][25]['colspan']=5;
	$_preview_cal[7][25]['background']='pspr_sb';
	$_preview_cal[7][25]['ltextz']='<div style="position:relative;left:4px;top:15px;">';
	$_preview_cal[7][25]['rtextz']='</div>';
	$_preview_cal[7][25]['text']='TP2';
	$_preview_cal[7][25]['pkl']='psprl';
	$_preview_cal[7][25]['ltext']='<img src="'.$GLOBALS['gfxpath'].'psprl.png" align="left" valign="top">';
	$_preview_cal[7][25]['pkr']='psprr';
	$_preview_cal[7][25]['rtext']='<img src="'.$GLOBALS['gfxpath'].'psprr.png" align="right" valign="top">';
	$_preview_cal[7][25]['align']='left" valign="top';
	$_preview_cal[7][25]['tooltip_js']='<div class=rhd>50.2&nbsp;-&nbsp;TP2</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Soll-Zeitraum:</b></td><td nowrap>26.08.2013&nbsp;-&nbsp;30.08.2013</td></tr><tr><td nowrap><b>Projektstatus:</b></td><td nowrap>geplant</td></tr></table>';
	$_preview_cal[7][25]['link']='?tr=23&aktion=1&gproject=23197';
	$_preview_cal[7][25]['anchor']=true;
	$_preview_cal[7][25]['status']=297;
	$_preview_cal[7][25]['mode']='010';


	$_preview_cal[8][0]['text']='<input type="hidden" id="disp23198" name="disp23198" fxform="_fxform_selmask_1" value="_"><input type="hidden" id="pr[8]" name="pr[8]" fxform="_fxform_selmask_1" value="23198"><input type="hidden" id="vn[8]" name="vn[8]" fxform="_fxform_selmask_1" value="0050.0002.0001"><img src="'.$GLOBALS['gfxpath'].'dmy.png" width="13" height="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt61.png" align="middle">&nbsp;<font class="black">50.2.1</font>&nbsp;<font class="darkgrey">A2</font>&nbsp;';
	$_preview_cal[8][0]['class']='yat0';
	$_preview_cal[8][0]['disp']='(leer)';
	$_preview_cal[8][0]['typ']='All';
	$_preview_cal[8][0]['sk']=25;
	$_preview_cal[8][0]['ek']=29;

	$_preview_cal[8][1]['text']='<a href="#pj6">&nbsp;<i class="grey">(26.08.2013&nbsp;-&nbsp;30.08.2013)</i>&nbsp;<br></a>';
	$_preview_cal[8][1]['class']='yat0';
	$_preview_cal[8][1]['align']='center';
	$_preview_cal[8][1]['typ']='All';

	$_preview_cal[8][2]['align']='center';
	$_preview_cal[8][2]['tooltip_js']='Das geplante Projektende wurde bereits überschritten. Die Aufgabe wurde noch nicht begonnen!';
	$_preview_cal[8][2]['text']='<img src="'.$GLOBALS['gfxpath'].'c_msg.png" style="margin:2px;margin-top:2px;">';
	$_preview_cal[8][2]['typ']='All';
	$_preview_cal[8][2]['class']='yat0';

	$_preview_cal[8][3]['text']='<span style="padding:0 2px;"><img id="report8" name="report8" class="fxfil" src=""></span>';
	$_preview_cal[8][3]['align']='right';
	$_preview_cal[8][3]['class']='yat0';
	$_preview_cal[8][3]['typ']='All';

	$_preview_cal[8][4]['colspan']=21;
	$_preview_cal[8][4]['color']='d4d4cf';

	$_preview_cal[8][25]['class']='krz0';
	$_preview_cal[8][25]['colspan']=5;
	$_preview_cal[8][25]['background']='psau_sb';
	$_preview_cal[8][25]['ltextz']='<div style="position:relative;left:4px;top:15px;">';
	$_preview_cal[8][25]['rtextz']='</div>';
	$_preview_cal[8][25]['text']='A2';
	$_preview_cal[8][25]['pkl']='psau297l';
	$_preview_cal[8][25]['ltext']='<img src="'.$GLOBALS['gfxpath'].'psau297l.png" align="left" valign="top">';
	$_preview_cal[8][25]['pkr']='psau297r';
	$_preview_cal[8][25]['rtext']='<img src="'.$GLOBALS['gfxpath'].'psau297r.png" align="right" valign="top">';
	$_preview_cal[8][25]['align']='left" valign="top';
	$_preview_cal[8][25]['tooltip_js']='<div class=rhd>50.2.1&nbsp;-&nbsp;A2</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Soll-Zeitraum:</b></td><td nowrap>26.08.2013&nbsp;-&nbsp;30.08.2013</td></tr><tr><td nowrap><b>Projektstatus:</b></td><td nowrap>geplant</td></tr></table>';
	$_preview_cal[8][25]['link']='?tr=23&aktion=1&gproject=23198';
	$_preview_cal[8][25]['anchor']=true;
	$_preview_cal[8][25]['status']=297;
	$_preview_cal[8][25]['mode']='010';

	// English
	if($lng != 1)
	{
		$_preview_cal[0][0]['text']='&nbsp;<b>Project Status</b>:&nbsp;';
		$_preview_cal[0][1]['text']='&nbsp;All Projects&nbsp;';

		$_preview_cal[1][0]['text']='&nbsp;<b>Timespan</b>:&nbsp;';
		$_preview_cal[1][1]['text']='&nbsp;08/05/2013&nbsp;-&nbsp;08/30/2013&nbsp;';
		$_preview_cal[1][4]['text']='CW 32';
		$_preview_cal[1][11]['text']='CW 33';
		$_preview_cal[1][18]['text']='CW 34';
		$_preview_cal[1][25]['text']='CW 35';

		$_preview_cal[2][0]['text']='&nbsp;<b>Project Name</b>&nbsp;';
		$_preview_cal[2][1]['text']='&nbsp;<b>Project Duration</b>&nbsp;';

		$_preview_cal[3][0]['disp']='(empty)';
		$_preview_cal[3][1]['text']='<a href="#pj1">&nbsp;<i class="grey">(08/05/2013&nbsp;-&nbsp;08/30/2013)</i>&nbsp;<br></a>';
		$_preview_cal[3][4]['tooltip_js']='<div class=rhd>50&nbsp;-&nbsp;MP</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Planned Timespan:</b></td><td nowrap>08/05/2013&nbsp;-&nbsp;08/30/2013</td></tr><tr><td nowrap><b>Project Status:</b></td><td nowrap>planned</td></tr></table>';

		$_preview_cal[4][0]['text']='<input type="hidden" id="disp23194" name="disp23194" fxform="_fxform_selmask_1" value="_cl"><input type="hidden" id="pr[4]" name="pr[4]" fxform="_fxform_selmask_1" value="23194"><input type="hidden" id="vn[4]" name="vn[4]" fxform="_fxform_selmask_1" value="0050.0001"><img id="its4" src="'.$GLOBALS['gfxpath'].'fl_cl.png" hspace="2" style="cursor:pointer;" '.fxf_jsFunction('toggleSubs', 4, '0050.0001').'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt60.png" align="middle">&nbsp;<font class="darkgrey">50.1</font>&nbsp;<font class="grey">SP1</font>&nbsp;';
		$_preview_cal[4][0]['disp']='(empty)';
		$_preview_cal[4][1]['text']='<a href="#pj2">&nbsp;<i class="grey">(08/05/2013&nbsp;-&nbsp;08/23/2013)</i>&nbsp;<br></a>';
		$_preview_cal[4][4]['text']='SP1';
		$_preview_cal[4][4]['tooltip_js']='<div class=rhd>50.1&nbsp;-&nbsp;SP1</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Planned Timespan:</b></td><td nowrap>08/05/2013&nbsp;-&nbsp;08/23/2013</td></tr><tr><td nowrap><b>Project Status:</b></td><td nowrap>planned</td></tr></table>';

		$_preview_cal[5][0]['text']='<input type="hidden" id="disp23195" name="disp23195" fxform="_fxform_selmask_1" value="_"><input type="hidden" id="pr[5]" name="pr[5]" fxform="_fxform_selmask_1" value="23195"><input type="hidden" id="vn[5]" name="vn[5]" fxform="_fxform_selmask_1" value="0050.0001.0001"><img src="'.$GLOBALS['gfxpath'].'dmy.png" width="13" height="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt61.png" align="middle">&nbsp;<font class="black">50.1.1</font>&nbsp;<font class="darkgrey">T1-1</font>&nbsp;';
		$_preview_cal[5][0]['disp']='(empty)';
		$_preview_cal[5][1]['text']='<a href="#pj3">&nbsp;<i class="grey">(08/05/2013&nbsp;-&nbsp;08/16/2013)</i>&nbsp;<br></a>';
		$_preview_cal[5][4]['text']='T1-1';
		$_preview_cal[5][4]['tooltip_js']='<div class=rhd>50.1.1&nbsp;-&nbsp;T1-1</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Planned Timespan:</b></td><td nowrap>08/05/2013&nbsp;-&nbsp;08/16/2013</td></tr><tr><td nowrap><b>Project Status:</b></td><td nowrap>planned</td></tr></table>';

		$_preview_cal[6][0]['text']='<input type="hidden" id="disp23196" name="disp23196" fxform="_fxform_selmask_1" value="_"><input type="hidden" id="pr[6]" name="pr[6]" fxform="_fxform_selmask_1" value="23196"><input type="hidden" id="vn[6]" name="vn[6]" fxform="_fxform_selmask_1" value="0050.0001.0002"><img src="'.$GLOBALS['gfxpath'].'dmy.png" width="13" height="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt61.png" align="middle">&nbsp;<font class="black">50.1.2</font>&nbsp;<font class="darkgrey">T1-2</font>&nbsp;';
		$_preview_cal[6][0]['disp']='(empty)';
		$_preview_cal[6][0]['mst'][0]['name']='Milestone: End Part 1 (08/23/2013)';
		$_preview_cal[6][1]['text']='<a href="#pj4">&nbsp;<i class="grey">(08/12/2013&nbsp;-&nbsp;08/23/2013)</i>&nbsp;<br></a>';
		$_preview_cal[6][11]['text']='T1-2';
		$_preview_cal[6][11]['tooltip_js']='<div class=rhd>50.1.2&nbsp;-&nbsp;T1-2</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Planned Timespan:</b></td><td nowrap>08/12/2013&nbsp;-&nbsp;08/23/2013</td></tr><tr><td nowrap><b>Milestone:</b></td><td nowrap>End Part 1 (08/23/2013)<br></td></tr><tr><td nowrap><b>Project Status:</b></td><td nowrap>planned</td></tr></table>';

		$_preview_cal[7][0]['text']='<input type="hidden" id="disp23197" name="disp23197" fxform="_fxform_selmask_1" value="_cl"><input type="hidden" id="pr[7]" name="pr[7]" fxform="_fxform_selmask_1" value="23197"><input type="hidden" id="vn[7]" name="vn[7]" fxform="_fxform_selmask_1" value="0050.0002"><img id="its7" src="'.$GLOBALS['gfxpath'].'fl_cl.png" hspace="2" style="cursor:pointer;" '.fxf_jsFunction('toggleSubs', 7, '0050.0002').'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt60.png" align="middle">&nbsp;<font class="darkgrey">50.2</font>&nbsp;<font class="grey">SP2</font>&nbsp;';
		$_preview_cal[7][0]['disp']='(empty)';
		$_preview_cal[7][1]['text']='<a href="#pj5">&nbsp;<i class="grey">(08/26/2013&nbsp;-&nbsp;08/30/2013)</i>&nbsp;<br></a>';
		$_preview_cal[7][25]['text']='SP2';
		$_preview_cal[7][25]['tooltip_js']='<div class=rhd>50.2&nbsp;-&nbsp;SP2</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Planned Timespan:</b></td><td nowrap>08/26/2013&nbsp;-&nbsp;08/30/2013</td></tr><tr><td nowrap><b>Project Status:</b></td><td nowrap>planned</td></tr></table>';

		$_preview_cal[8][0]['text']='<input type="hidden" id="disp23198" name="disp23198" fxform="_fxform_selmask_1" value="_"><input type="hidden" id="pr[8]" name="pr[8]" fxform="_fxform_selmask_1" value="23198"><input type="hidden" id="vn[8]" name="vn[8]" fxform="_fxform_selmask_1" value="0050.0002.0001"><img src="'.$GLOBALS['gfxpath'].'dmy.png" width="13" height="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'pt61.png" align="middle">&nbsp;<font class="black">50.2.1</font>&nbsp;<font class="darkgrey">T2</font>&nbsp;';
		$_preview_cal[8][0]['disp']='(empty)';
		$_preview_cal[8][1]['text']='<a href="#pj6">&nbsp;<i class="grey">(08/26/2013&nbsp;-&nbsp;08/30/2013)</i>&nbsp;<br></a>';
		$_preview_cal[8][25]['text']='T2';
		$_preview_cal[8][25]['tooltip_js']='<div class=rhd>50.2.1&nbsp;-&nbsp;T2</div><table border=0 cellpadding=1 cellspacing=0><tr><td nowrap><b>Planned Timespan:</b></td><td nowrap>08/26/2013&nbsp;-&nbsp;08/30/2013</td></tr><tr><td nowrap><b>Project Status:</b></td><td nowrap>planned</td></tr></table>';
	}

	$_preview_tr=26;

	$incfn='d_manager_pst';
}
else if($_POST['ktyp'] == 2019)	// Leistungsnachweis
{
	// English
	$_preview_header[0]			= '&nbsp;Day&nbsp;';
	$_preview_header[1]			= '&nbsp;Date&nbsp;';
	$_preview_header[2]			= '&nbsp;Time&nbsp;';
	$_preview_header[3]			= '&nbsp;Break&nbsp;';
	$_preview_header[4]			= '&nbsp;Project&nbsp;';
	$_preview_header[6]			= '&nbsp;Activity&nbsp;';
	$_preview_header[8]			= '&nbsp;Tot.Hrs.&nbsp;';

	$_preview_madd=array();
	$_preview_madd[1][1]		= 0;
	$_preview_madd[1][2]		= 1;
	$_preview_madd[1][3]		= 2;
	$_preview_madd[1][4]		= 3;
	$_preview_madd[1][5]		= 4;
	$_preview_madd[1][6]		= 'blank';
	$_preview_madd[1][7]		= 6;
	$_preview_madd[1][8]		= 'blank';
	$_preview_madd[1][9]		= 8;
	$_preview_madd[1][10]		= 'blank';
	$_preview_madd[1][11]		= 'blank';

	$_preview_madd['lines']		= 1;
	$_preview_madd['columns']	= '1,2,3,4,5,7,9';

	$_preview_madd['rep_headline']	= 'Performance Proof Test';

	$_preview_values=array();
	$_preview_values[0]['tage'] 			= '~Mo';
	$_preview_values[0]['datum'] 			= '~08/03/2013';
	$_preview_values[0]['zeit_id'] 			= '~08:00-17:00';
	$_preview_values[0]['pause'] 			= 1800;
	$_preview_values[0]['projekt'] 			= '~50.1.1&nbsp;<font class="darkgrey">T1-1</font>';
	$_preview_values[0]['taetigkeit_text'] 	= 'Work 1';
	$_preview_values[0]['ges_std'] 			= '30600^';

	$_preview_values[1]['tage'] 			= '~Tu';
	$_preview_values[1]['datum'] 			= '~08/04/2013';
	$_preview_values[1]['zeit_id'] 			= '~08:30-17:00';
	$_preview_values[1]['pause'] 			= 1800;
	$_preview_values[1]['projekt'] 			= '~50.1.1&nbsp;<font class="darkgrey">T1-1</font>';
	$_preview_values[1]['taetigkeit_text'] 	= 'Work 2';
	$_preview_values[1]['ges_std'] 			= '28800^';

	$_preview_values[2]['tage'] 			= '~We';
	$_preview_values[2]['datum'] 			= '~08/05/2013';
	$_preview_values[2]['zeit_id'] 			= '~08:00-17:00';
	$_preview_values[2]['pause'] 			= 1800;
	$_preview_values[2]['projekt'] 			= '~50.1.2&nbsp;<font class="darkgrey">T1-2</font>';
	$_preview_values[2]['taetigkeit_text'] 	= 'Work 3';
	$_preview_values[2]['ges_std'] 			= '30600^';

	$_preview_values[3]['tage'] 			= '~Th';
	$_preview_values[3]['datum'] 			= '~08/06/2013';
	$_preview_values[3]['zeit_id'] 			= '~08:30-17:00';
	$_preview_values[3]['pause'] 			= 1800;
	$_preview_values[3]['projekt'] 			= '~50.1.2&nbsp;<font class="darkgrey">T1-2</font>';
	$_preview_values[3]['taetigkeit_text'] 	= 'Work 4';
	$_preview_values[3]['ges_std'] 			= '28800^';

	$_preview_values[4]['tage'] 			= '~Fr';
	$_preview_values[4]['datum'] 			= '~08/07/2013';
	$_preview_values[4]['zeit_id'] 			= '~08:30-17:00';
	$_preview_values[4]['pause'] 			= 1800;
	$_preview_values[4]['projekt'] 			= '~50.2.1&nbsp;<font class="darkgrey">T2</font>';
	$_preview_values[4]['taetigkeit_text'] 	= 'Work 5';
	$_preview_values[4]['ges_std'] 			= '28800^';

	$_preview_values[5]['tage'] 			= '~|';
	$_preview_values[5]['datum'] 			= '~|';
	$_preview_values[5]['zeit_id'] 			= '~|';
	$_preview_values[5]['pause'] 			= '~|';
	$_preview_values[5]['projekt'] 			= '~<b>Grand-total</b>|z';
	$_preview_values[5]['taetigkeit_text'] 	= '~^|z';
	$_preview_values[5]['ges_std'] 			= '147600|z';

	// Deutsch
	if($lng == 1)
	{
		$_preview_header[0]						= '&nbsp;Tag&nbsp;';
		$_preview_header[1]						= '&nbsp;Datum&nbsp;';
		$_preview_header[2]						= '&nbsp;Zeit&nbsp;';
		$_preview_header[3]						= '&nbsp;Pause&nbsp;';
		$_preview_header[4]						= '&nbsp;Projekt&nbsp;';
		$_preview_header[6]						= '&nbsp;Tätigkeit&nbsp;';
		$_preview_header[8]						= '&nbsp;Ges.-Std.&nbsp;';

		$_preview_madd['rep_headline']			= 'Leistungsnachweis Test';

		$_preview_values[0]['tage'] 			= '~Mo';
		$_preview_values[0]['datum'] 			= '~03.08.2013';
		$_preview_values[0]['projekt'] 			= '~50.1.1&nbsp;<font class="darkgrey">A1-1</font>';
		$_preview_values[0]['taetigkeit_text'] 	= 'Tätigkeit 1';

		$_preview_values[1]['tage'] 			= '~Di';
		$_preview_values[1]['datum'] 			= '~04.08.2013';
		$_preview_values[1]['projekt'] 			= '~50.1.1&nbsp;<font class="darkgrey">A1-1</font>';
		$_preview_values[1]['taetigkeit_text'] 	= 'Tätigkeit 2';

		$_preview_values[2]['tage'] 			= '~Mi';
		$_preview_values[2]['datum'] 			= '~05.08.2013';
		$_preview_values[2]['projekt'] 			= '~50.1.2&nbsp;<font class="darkgrey">A1-2</font>';
		$_preview_values[2]['taetigkeit_text'] 	= 'Tätigkeit 3';

		$_preview_values[3]['tage'] 			= '~Do';
		$_preview_values[3]['datum'] 			= '~06.08.2013';
		$_preview_values[3]['projekt'] 			= '~50.1.2&nbsp;<font class="darkgrey">A1-2</font>';
		$_preview_values[3]['taetigkeit_text'] 	= 'Tätigkeit 4';

		$_preview_values[4]['tage'] 			= '~Fr';
		$_preview_values[4]['datum'] 			= '~07.08.2013';
		$_preview_values[4]['projekt'] 			= '~50.2.1&nbsp;<font class="darkgrey">A2</font>';
		$_preview_values[4]['taetigkeit_text'] 	= 'Tätigkeit 5';

		$_preview_values[5]['projekt'] 			= '~<b>Gesamtsumme</b>|z';
	}

	$_preview_tr		= 114;
	$_preview_mask		= 294;
	$_preview_headline	= $_preview_madd['rep_headline'];

	$incfn='d_manager';
}
else if($_POST['ktyp'] == 2021)	// Rechnung
{
	$sv_vorgang_zu_bezeichnung	= false;
	$sv_keine_mwst_spalten		= false;
	$sv_col_total				= 'class_bcr3';
	foreach($_preview_config as $_preview_config_line)
	{
		$_preview_config_line = trim($_preview_config_line);
		if(substr($_preview_config_line, 0, 2) == '//')
			continue;

		$kto_pos = strpos($_preview_config_line, '#kto_');
		if($kto_pos !== false)
		{
			$kto_name = trim(substr($_preview_config_line, $kto_pos+5));
			if($kto_name == 'rechnungstyp')
				$kto_name = '2121';
			else if($kto_name == 'rechnungsanschrift')
				$kto_name = '2113';
			if(strlen($_preview_invoice['kto']))
				$_preview_invoice['kto'] .= ',';
			$_preview_invoice['kto'] .= $kto_name;
		}

		$ktu_pos = strpos($_preview_config_line, '#ktu_');
		if($ktu_pos !== false)
		{
			$ktu_name = trim(substr($_preview_config_line, $ktu_pos+5));
			if(strlen($_preview_invoice['ktu']))
				$_preview_invoice['ktu'] .= ',';
			$_preview_invoice['ktu'] .= $ktu_name;
		}

		$ip_pos = strpos($_preview_config_line, '#_intprocess');
		if($ip_pos !== false)
		{
			$ip_val = (int)trim(substr($_preview_config_line, $ip_pos+12));
			if($ip_val)
				$sv_vorgang_zu_bezeichnung = true;
		}

		$nv_pos = strpos($_preview_config_line, '#_novat');
		if($nv_pos !== false)
		{
			$nv_val = (int)trim(substr($_preview_config_line, $nv_pos+7));
			if($nv_val)
				$sv_keine_mwst_spalten = true;
		}

		$ct_pos = strpos($_preview_config_line, '#col_total');
		if($ct_pos !== false)
		{
			$ct_val = trim(substr($_preview_config_line, $ct_pos+10));
			if(strlen($ct_val) && (substr($ct_val, 0, 1) == '#'))
				$sv_col_total = 'col_'.substr($ct_val, 1, 6);
		}
	}

	// English
	$_preview_header[0]			= '&nbsp;Pos.&nbsp;';
	$_preview_header[1]			= '&nbsp;Project&nbsp;';
	$_preview_header[2]			= '&nbsp;Description&nbsp;';
	$_preview_header[3]			= '&nbsp;Amount&nbsp;';
	$_preview_header[4]			= '&nbsp;Unit&nbsp;';
	$_preview_header[5]			= '&nbsp;VAT&nbsp;';
	$_preview_header[6]			= '&nbsp;UP Net&nbsp;';
	$_preview_header[7]			= '&nbsp;TP Net&nbsp;';
	$_preview_header[8]			= '&nbsp;TP VAT&nbsp;';
	$_preview_header[9]			= '&nbsp;TP Gross&nbsp;';

	$_preview_madd[1][1]		= 0;
	$_preview_madd[1][2]		= 1;
	$_preview_madd[1][3]		= 2;
	$_preview_madd[1][4]		= 3;
	$_preview_madd[1][5]		= 4;
	$_preview_madd[1][6]		= 5;
	$_preview_madd[1][7]		= 6;
	$_preview_madd[1][8]		= 7;
	$_preview_madd[1][9]		= 8;
	$_preview_madd[1][10]		= 9;

	$_preview_madd['lines']		= 1;
	$_preview_madd['columns']	= '1,2,3,4,5,6,7,8,9,10';

	$_preview_madd['rep_headline']					= 'Invoice Test';

	$_preview_values[0]['positions_id']				= '~1';
	$_preview_values[0]['name_projekt']				= '~<font color="00344e">[50.1.1] T1-1</font>';
	$_preview_values[0]['r_bezeichnung']			= '~<font color="#00344e">Doe, John</font>';
	$_preview_values[0]['r_anzahl']					= '~18.00';
	$_preview_values[0]['verkaufseinheit']			= '~h';
	$_preview_values[0]['mwst_satz']				= '~19.00&nbsp;%';
	$_preview_values[0]['r_einzelpreis']			= '~70.00&nbsp;$';
	$_preview_values[0]['r_gesamtpreis']			= '~1,260.00&nbsp;$';
	$_preview_values[0]['r_gesamtmwst']				= '~239.40&nbsp;$';
	$_preview_values[0]['r_gesamtbrutto']			= '~1,499.40&nbsp;$';

	$_preview_values[1]['positions_id']				= '~|';
	$_preview_values[1]['name_projekt']				= '~|';
	$_preview_values[1]['r_bezeichnung']			= '^class_rv0" colspan="8" nowrap><!--|~<i class="grey">(Optional field for notes)</i>';
	$_preview_values[1]['r_gesamtbrutto']			= '~--><i class="grey">(Optional field for notes)</i>';

	$_preview_values[2]['positions_id']				= '~2';
	$_preview_values[2]['name_projekt']				= '~<font color="00344e">Unknown</font>';
	$_preview_values[2]['r_bezeichnung']			= '~<font color="#00344e">Doe, Jane</font>';
	$_preview_values[2]['r_anzahl']					= '~8.00';
	$_preview_values[2]['verkaufseinheit']			= '~h';
	$_preview_values[2]['mwst_satz']				= '~19.00&nbsp;%';
	$_preview_values[2]['r_einzelpreis']			= '~70.00&nbsp;$';
	$_preview_values[2]['r_gesamtpreis']			= '~560.00&nbsp;$';
	$_preview_values[2]['r_gesamtmwst']				= '~106.40&nbsp;$';
	$_preview_values[2]['r_gesamtbrutto']			= '~666.40&nbsp;$';

	$_preview_values[3]['positions_id']				= '^'.$sv_col_total.'" colspan="7" nowrap><!--|~&nbsp;<b>Grand-total</b>';
	$_preview_values[3]['r_einzelpreis']			= '~-->&nbsp;<b>Grant-total</b>';
	$_preview_values[3]['r_gesamtpreis']			= '^'.$sv_col_total.'|~1,820.00</b>&nbsp;$';
	$_preview_values[3]['r_gesamtmwst']				= '^'.$sv_col_total.'|~345.80</b>&nbsp;$';
	$_preview_values[3]['r_gesamtbrutto']			= '^'.$sv_col_total.'|~&nbsp;<b>2,165.80</b>&nbsp;$';

	$_preview_invoice['mandanten_id']				= 1;
	$_preview_invoice['rechnungs_id']				= -1;
	$_preview_invoice['rechnn_f']					= 'TEST-JJ-001';
	$_preview_invoice['datum']						= '20130812000000';
	$_preview_invoice['rechnungstyp']				= 0;
	$_preview_invoice['rechnungsform']				= '0/cfg_prng.cfg';
	$_preview_invoice['kalktyp']					= 2733;
	$_preview_invoice['buendelpos']					= 0;
	$_preview_invoice['kto']						= '';
	$_preview_invoice['ktu']						= '';
	$_preview_invoice['ist_beg_dtm']				= '20130801000000';
	$_preview_invoice['ist_end_dtm']				= '20130831240000';
	$_preview_invoice['kunde']						= 1;
	$_preview_invoice['mutterprojekt_id']			= -1;
	$_preview_invoice['name_mutterprojekt']			= 'Mother Project';
	$_preview_invoice['pname']						= 'Test Inc.';
	$_preview_invoice['vorname']					= '';
	$_preview_invoice['anredetext']					= 'Company';
	$_preview_invoice['kundennummer']				= 'TEST';
	$_preview_invoice['adressen_id']				= 1;
	$_preview_invoice['staat']						= 'GB';
	$_preview_invoice['plz']						= 'W2HA BK';
	$_preview_invoice['ort']						= 'London';
	$_preview_invoice['strasse']					= 'Main St. 123';
	$_preview_invoice['person_angelegt']			= 0;
	$_preview_invoice['allg_textfeld']				= 'Header text field - Test';
	$_preview_invoice['allg_textfeld_fuss']			= 'Footer text field - Test';
	$_preview_invoice['nettobetrag']				= 1820.00;
	$_preview_invoice['bruttobetrag']				= 2165.80;
	$_preview_invoice['mehrwertsteuer']				= 345.80;
	$_preview_invoice['ausgegangen_am']				= '';
	$_preview_invoice['bezahlt_am']					= '';
	$_preview_invoice['bezahlt']					= '';
	$_preview_invoice['archiv']						= '';
	$_preview_invoice['aenderungs_id']				= 1;
	$_preview_invoice['zeitstempel']				= '20130901140000';

	$_preview_invoice_contact						= 'Mr. John Doe';

	if($sv_vorgang_zu_bezeichnung || $sv_keine_mwst_spalten)
	{
		$_preview_values[3]['positions_id']				= '^'.$sv_col_total.'" colspan="6" nowrap><!--|~&nbsp;<b>Grand-total</b>';
		if($sv_vorgang_zu_bezeichnung && $sv_keine_mwst_spalten)
		{
			$_preview_madd['columns']	= '1,3,4,5,7,8';
			$_preview_values[3]['positions_id']				= '^'.$sv_col_total.'" colspan="5" nowrap><!--|~&nbsp;<b>Grand-total</b>';
		}
		else if($sv_vorgang_zu_bezeichnung)
		{
			$_preview_madd['columns']	= '1,3,4,5,6,7,8,9,10';
		}
		else
		{
			$_preview_madd['columns']	= '1,2,3,4,5,7,8';
		}

		if($sv_vorgang_zu_bezeichnung)
		{
			$_preview_values[0]['r_bezeichnung']			= '~<font color="#7788EE"><b>Doe, John</b></font><br><font color="#333333"><b>Project</b>:<br>[50.1.1] T1-1</font>';
			$_preview_values[2]['r_bezeichnung']			= '~<font color="#7788EE"><b>Doe, Jane</b></font><br><font color="#333333"><b>Project</b>:<br>Unknown</font>';
		}

		if($sv_keine_mwst_spalten)
		{
			$_preview_values[1]['r_bezeichnung']			= '^class_rv0" colspan="5" nowrap><!--|~<i class="grey">(Optional field for notes)</i>';
			$_preview_values[1]['r_gesamtpreis']			= '~--><i class="grey">(Optional field for notes)</i>';
			unset($_preview_values[1]['r_gesamtbrutto']);
		}
	}

	// Deutsch
	if($lng == 1)
	{
		$_preview_header[0]								= '&nbsp;Pos.&nbsp;';
		$_preview_header[1]								= '&nbsp;Projekt&nbsp;';
		$_preview_header[2]								= '&nbsp;Bezeichnung&nbsp;';
		$_preview_header[3]								= '&nbsp;Menge&nbsp;';
		$_preview_header[4]								= '&nbsp;Einheit&nbsp;';
		$_preview_header[5]								= '&nbsp;MwSt.&nbsp;';
		$_preview_header[6]								= '&nbsp;EP Netto&nbsp;';
		$_preview_header[7]								= '&nbsp;GP Netto&nbsp;';
		$_preview_header[8]								= '&nbsp;GP MwSt.&nbsp;';
		$_preview_header[9]								= '&nbsp;GP Brutto&nbsp;';

		$_preview_madd['rep_headline']					= 'Rechnung Test';

		$_preview_values[0]['positions_id']				= '~1';
		$_preview_values[0]['name_projekt']				= '~<font color="#00344e">[50.1.1] A1-1</font>';
		$_preview_values[0]['r_bezeichnung']			= '~<font color="#00344e">Mustermann, Max</font>';
		$_preview_values[0]['r_anzahl']					= '~18,00';
		$_preview_values[0]['verkaufseinheit']			= '~Std.';
		$_preview_values[0]['mwst_satz']				= '~19,00&nbsp;%';
		$_preview_values[0]['r_einzelpreis']			= '~70,00&nbsp;';
		$_preview_values[0]['r_gesamtpreis']			= '~1.260,00&nbsp;';
		$_preview_values[0]['r_gesamtmwst']				= '~239,40&nbsp;';
		$_preview_values[0]['r_gesamtbrutto']			= '~1.499,40&nbsp;';

		$_preview_values[1]['positions_id']				= '~|';
		$_preview_values[1]['name_projekt']				= '~|';
		$_preview_values[1]['r_bezeichnung']			= '^class_rv0" colspan="8" nowrap><!--|~<i class="grey">(optionales Anmerkungsfeld)</i>';
		$_preview_values[1]['r_gesamtbrutto']			= '~--><i class="grey">(optionales Anmerkungsfeld)</i>';

		$_preview_values[2]['positions_id']				= '~2';
		$_preview_values[2]['name_projekt']				= '~<font color="#00344e">Unbekannt</font>';
		$_preview_values[2]['r_bezeichnung']			= '~<font color="#00344e">Musterfrau, Maria</font>';
		$_preview_values[2]['r_anzahl']					= '~8,00';
		$_preview_values[2]['verkaufseinheit']			= '~Std.';
		$_preview_values[2]['mwst_satz']				= '~19,00&nbsp;%';
		$_preview_values[2]['r_einzelpreis']			= '~70,00&nbsp;';
		$_preview_values[2]['r_gesamtpreis']			= '~560,00&nbsp;';
		$_preview_values[2]['r_gesamtmwst']				= '~106,40&nbsp;';
		$_preview_values[2]['r_gesamtbrutto']			= '~666,40&nbsp;';

		$_preview_values[3]['positions_id']				= '^'.$sv_col_total.'" colspan="7" nowrap><!--|~&nbsp;<b>Gesamtsumme</b>';
		$_preview_values[3]['r_einzelpreis']			= '~-->&nbsp;<b>Gesamtsumme</b>';
		$_preview_values[3]['r_gesamtpreis']			= '^'.$sv_col_total.'|~1.820,00&nbsp;';
		$_preview_values[3]['r_gesamtmwst']				= '^'.$sv_col_total.'|~345,80&nbsp;';
		$_preview_values[3]['r_gesamtbrutto']			= '^'.$sv_col_total.'|~<b>2.165,80</b>&nbsp;';

		$_preview_invoice['name_mutterprojekt']			= 'Mutterprojekt';
		$_preview_invoice['pname']						= 'Test GmbH';
		$_preview_invoice['anredetext']					= 'Firma';
		$_preview_invoice['kundennummer']				= 'TEST';
		$_preview_invoice['staat']						= 'D';
		$_preview_invoice['plz']						= '12345';
		$_preview_invoice['ort']						= 'Musterort';
		$_preview_invoice['strasse']					= 'Musterstr. 123';
		$_preview_invoice['allg_textfeld']				= 'Kopf Textfeld - Test';
		$_preview_invoice['allg_textfeld_fuss']			= 'Fuss Textfeld - Test';

		$_preview_invoice_contact						= 'Herr Max Mustermann';

		if($sv_vorgang_zu_bezeichnung || $sv_keine_mwst_spalten)
		{
			$_preview_values[3]['positions_id']				= '^'.$sv_col_total.'" colspan="6" nowrap><!--|~&nbsp;<b>Gesamtsumme</b>';
			if($sv_vorgang_zu_bezeichnung && $sv_keine_mwst_spalten)
				$_preview_values[3]['positions_id']				= '^'.$sv_col_total.'" colspan="5" nowrap><!--|~&nbsp;<b>Gesamtsumme</b>';

			if($sv_vorgang_zu_bezeichnung)
			{
				$_preview_values[0]['r_bezeichnung']			= '~<font color="#7788EE"><b>Mustermann, Max</b></font><br><font color="#333333"><b>Projekt</b>:<br>[50.1.1] A1-1</font>';
				$_preview_values[2]['r_bezeichnung']			= '~<font color="#7788EE"><b>Musterfrau, Maria</b></font><br><font color="#333333"><b>Projekt</b>:<br>Unbekannt</font>';
			}

			if($sv_keine_mwst_spalten)
			{
				$_preview_values[1]['r_bezeichnung']			= '^class_rv0" colspan="5" nowrap><!--|~<i class="grey">(optionales Anmerkungsfeld)</i>';
				$_preview_values[1]['r_gesamtpreis']			= '~--><i class="grey">(optionales Anmerkungsfeld)</i>';
			}
		}
	}

	$_preview_invid	= -1;
	$_preview_tr	= 104;
	$_preview_mask	= 277;

	$incfn='d_manager';
}
else if($_POST['ktyp'] == 2819)	// Reisekosten
{
	// English
	$_preview_madd[1][10]		= 0;	// Tage
	$_preview_madd[1][20]		= 1;	// Datum
	$_preview_madd[1][30]		= 2;	// Zeitraum
	$_preview_madd[1][40]		= 3;	// stunden
	$_preview_madd[1][95]		= 4;	// nv_text1
	$_preview_madd[1][100]		= 5;	// pverpflegung
	$_preview_madd[1][110]		= 6;	// pnacht
	$_preview_madd[1][190]		= 7;	// SumTag
	$_preview_madd[1][195]		= 8;	// nv_text2
	$_preview_madd[1][200]		= 9;	// pfrueh
	$_preview_madd[1][210]		= 10;	// pmittag
	$_preview_madd[1][220]		= 11;	// pabend
	$_preview_madd[1][290]		= 12;	// SumIst

	$_preview_madd['lines']		= 1;
	$_preview_madd['columns']	= '10,20,30,40,95,100,110,190,195,200,210,220,290';

	$_preview_values[0]['tage']='^class_rhl" nowrap align="center" colspan="4"><!--|~&nbsp;Days&nbsp;';
	$_preview_values[0]['stunden']='~-->&nbsp;Days&nbsp;';
	$_preview_values[0]['pverpflegung']='^class_rhl" nowrap align="center" colspan="3" style="border-left:1px solid #888888;"><!--|~&nbsp;Daily Allowances&nbsp;';
	$_preview_values[0]['sumtag']='~-->&nbsp;Daily Allowances&nbsp;';
	$_preview_values[0]['pfrueh']='^class_rhl" nowrap align="center" colspan="4" style="border-left:1px solid #888888;"><!--|~&nbsp;Meals initiated by the employer&nbsp;';
	$_preview_values[0]['sumist']='~-->&nbsp;Meals initiated by the employer&nbsp;';

	$_preview_values[1]['tage']='^class_rhd" nowrap align="center" colspan="2"><!--|~&nbsp;Date&nbsp;';
	$_preview_values[1]['datum']='~-->&nbsp;Date&nbsp;';
	$_preview_values[1]['zeitraum']='^class_rhd" nowrap align="center|~&nbsp;Time Per.&nbsp;';
	$_preview_values[1]['stunden']='^class_rhd" nowrap align="center|~&nbsp;Hours&nbsp;';
	$_preview_values[1]['pverpflegung']='^class_rhd" nowrap align="center|~&nbsp;Board&nbsp;';
	$_preview_values[1]['pnacht']='^class_rhd" nowrap align="center|~&nbsp;Accom.&nbsp;';
	$_preview_values[1]['sumtag']='^class_rhd" nowrap align="center|~&nbsp;Sum&nbsp;';
	$_preview_values[1]['pfrueh']='^class_rhd" nowrap align="center|~&nbsp;Breakf.&nbsp;';
	$_preview_values[1]['pmittag']='^class_rhd" nowrap align="center|~&nbsp;Lunch&nbsp;';
	$_preview_values[1]['pabend']='^class_rhd" nowrap align="center|~&nbsp;Supper&nbsp;';
	$_preview_values[1]['sumist']='^class_rhd" nowrap align="center|~&nbsp;Sum&nbsp;';

	$_preview_values[2]['tage']='~|';
	$_preview_values[2]['datum']='~|';
	$_preview_values[2]['zeitraum']='~|';
	$_preview_values[2]['stunden']='~|';
	$_preview_values[2]['pverpflegung']='~|';
	$_preview_values[2]['pnacht']='~|';
	$_preview_values[2]['sumtag']='~|';
	$_preview_values[2]['pfrueh']='~|';
	$_preview_values[2]['pmittag']='~|';
	$_preview_values[2]['pabend']='~|';
	$_preview_values[2]['sumist']='~|';

	$_preview_values[3]['tage']='^col_e8e8e8" nowrap colspan="4" align="left"><!--|~&nbsp;<u><b>Inland Travel</b>&nbsp;[123]:</u>&nbsp;<br>&nbsp;Automobile (100&nbsp;km)&nbsp;';
	$_preview_values[3]['stunden']='~-->&nbsp;<u><b>Inland Travel</b>&nbsp;[123]:</u>&nbsp;<br>&nbsp;Automobile (100&nbsp;km)&nbsp;';
	$_preview_values[3]['pverpflegung']='^col_e8e8e8" nowrap colspan="3" align="left" style="border-left:1px solid #bbbbcc;"><!--|~&nbsp;<b>Time Per.:</b>&nbsp;<br>&nbsp;07/24/2013, 08:00&nbsp;-&nbsp;07/26/2013, 17:00&nbsp;';
	$_preview_values[3]['sumtag']='~-->&nbsp;<b>Time Per.:</b>&nbsp;<br>&nbsp;07/24/2013, 08:00&nbsp;-&nbsp;07/26/2013, 17:00&nbsp;';
	$_preview_values[3]['pfrueh']='^col_e8e8e8" nowrap colspan="4" align="left" style="border-left:1px solid #bbbbcc;"><!--|~&nbsp;<b>Start&nbsp;-&gt;&nbsp;Target:</b>&nbsp;<br>&nbsp;Here&nbsp;-&gt;&nbsp;There&nbsp;';
	$_preview_values[3]['sumist']='~-->&nbsp;<b>Start&nbsp;-&gt;&nbsp;Target:</b>&nbsp;<br>&nbsp;Here&nbsp;-&gt;&nbsp;There&nbsp;';

	$_preview_values[4]['tage']='~&nbsp;We';
	$_preview_values[4]['datum']='~&nbsp;07/24/2013&nbsp;';
	$_preview_values[4]['zeitraum']='~&nbsp;8:00&nbsp;-&nbsp;24:00&nbsp;';
	$_preview_values[4]['stunden']='~&nbsp;16.00&nbsp;h&nbsp;';
	$_preview_values[4]['pverpflegung']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_ja.png">&nbsp;';
	$_preview_values[4]['pnacht']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_nein.png">&nbsp;';
	$_preview_values[4]['sumtag']='~&nbsp;12.00&nbsp;$&nbsp;';
	$_preview_values[4]['pfrueh']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_ja.png">&nbsp;';
	$_preview_values[4]['pmittag']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_nein.png">&nbsp;';
	$_preview_values[4]['pabend']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_nein.png">&nbsp;';
	$_preview_values[4]['sumist']='~&nbsp;1.50&nbsp;$&nbsp;';

	$_preview_values[5]['tage']='~&nbsp;Th';
	$_preview_values[5]['datum']='~&nbsp;07/25/2013&nbsp;';
	$_preview_values[5]['zeitraum']='~&nbsp;0:00&nbsp;-&nbsp;24:00&nbsp;';
	$_preview_values[5]['stunden']='~&nbsp;24.00&nbsp;h&nbsp;';
	$_preview_values[5]['pverpflegung']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_ja.png">&nbsp;';
	$_preview_values[5]['pnacht']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_nein.png">&nbsp;';
	$_preview_values[5]['sumtag']='~&nbsp;24.00&nbsp;$&nbsp;';
	$_preview_values[5]['pfrueh']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_nein.png">&nbsp;';
	$_preview_values[5]['pmittag']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_nein.png">&nbsp;';
	$_preview_values[5]['pabend']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_nein.png">&nbsp;';
	$_preview_values[5]['sumist']='~&nbsp;0.00&nbsp;$&nbsp;';

	$_preview_values[6]['tage']='~&nbsp;Fr';
	$_preview_values[6]['datum']='~&nbsp;07/26/2013&nbsp;';
	$_preview_values[6]['zeitraum']='~&nbsp;0:00&nbsp;-&nbsp;17:00&nbsp;';
	$_preview_values[6]['stunden']='~&nbsp;17.00&nbsp;h&nbsp;';
	$_preview_values[6]['pverpflegung']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_ja.png">&nbsp;';
	$_preview_values[6]['pnacht']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_nein.png">&nbsp;';
	$_preview_values[6]['sumtag']='~&nbsp;12.00&nbsp;$&nbsp;';
	$_preview_values[6]['pfrueh']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_nein.png">&nbsp;';
	$_preview_values[6]['pmittag']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_nein.png">&nbsp;';
	$_preview_values[6]['pabend']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].'prf_nein.png">&nbsp;';
	$_preview_values[6]['sumist']='~&nbsp;0.00&nbsp;$&nbsp;';

	$_preview_values[7]['tage']='~|';
	$_preview_values[7]['datum']='~|';
	$_preview_values[7]['zeitraum']='~|';
	$_preview_values[7]['stunden']='~|';
	$_preview_values[7]['pverpflegung']='^class_bcr4" nowrap align="right" colspan="2"><!--|~&nbsp;<b>Daily Allowances:</b>&nbsp;';
	$_preview_values[7]['pnacht']='~-->&nbsp;<b>Daily Allowances:</b>&nbsp;';
	$_preview_values[7]['sumtag']='^class_bcr4|~&nbsp;<b>48.00</b>&nbsp;$&nbsp;';
	$_preview_values[7]['pfrueh']='^class_bcr4" nowrap align="right" colspan="3" style="border-left:1px solid #bbbbcc;"><!--|~&nbsp;<b>less Paym. in Kind:</b>&nbsp;';
	$_preview_values[7]['pabend']='~-->&nbsp;<b>less Paym. in Kind:</b>&nbsp;';
	$_preview_values[7]['sumist']='^class_bcr4|~&nbsp;<b>1.50</b>&nbsp;$&nbsp;';

	$_preview_values[8]['tage']='^class_bcr0" nowrap align="left" colspan="10"><!--|~<b>Sub-total</b>';
	$_preview_values[8]['pabend']='~--><b>Sub-total</b>';
	$_preview_values[8]['sumist']='^class_bcr0|~&nbsp;<b>46.50</b>&nbsp;$&nbsp;';

	$_preview_values[9]['tage']='~|';
	$_preview_values[9]['datum']='~|';
	$_preview_values[9]['zeitraum']='~|';
	$_preview_values[9]['stunden']='~|';
	$_preview_values[9]['pverpflegung']='~|';
	$_preview_values[9]['pnacht']='~|';
	$_preview_values[9]['sumtag']='~|';
	$_preview_values[9]['pfrueh']='~|';
	$_preview_values[9]['pmittag']='~|';
	$_preview_values[9]['pabend']='~|';
	$_preview_values[9]['sumist']='~|';

	$_preview_values[10]['tage']='^class_bcr3" nowrap align="left" colspan="10"><!--|~<b>Grand-total</b>';
	$_preview_values[10]['pabend']='~--><b>Grand-total</b>';
	$_preview_values[10]['sumist']='^class_bcr3|~&nbsp;<b>46.50</b>&nbsp;$&nbsp;';

	// Deutsch
	if($lng == 1)
	{
		$_preview_values[0]['tage']='^class_rhl" nowrap align="center" colspan="4"><!--|~&nbsp;Tage&nbsp;';
		$_preview_values[0]['stunden']='~-->&nbsp;Tage&nbsp;';
		$_preview_values[0]['pverpflegung']='^class_rhl" nowrap align="center" colspan="3" style="border-left:1px solid #888888;"><!--|~&nbsp;Tagespauschalen&nbsp;';
		$_preview_values[0]['sumtag']='~-->&nbsp;Tagespauschalen&nbsp;';
		$_preview_values[0]['pfrueh']='^class_rhl" nowrap align="center" colspan="4" style="border-left:1px solid #888888;"><!--|~&nbsp;Vom Arbeitgeber veranlasste Mahlzeiten&nbsp;';
		$_preview_values[0]['sumist']='~-->&nbsp;Vom Arbeitgeber veranlasste Mahlzeiten&nbsp;';

		$_preview_values[1]['tage']='^class_rhd" nowrap align="center" colspan="2"><!--|~&nbsp;Datum&nbsp;';
		$_preview_values[1]['datum']='~-->&nbsp;Datum&nbsp;';
		$_preview_values[1]['zeitraum']='^class_rhd" nowrap align="center|~&nbsp;Zeitraum&nbsp;';
		$_preview_values[1]['stunden']='^class_rhd" nowrap align="center|~&nbsp;Stunden&nbsp;';
		$_preview_values[1]['pverpflegung']='^class_rhd" nowrap align="center|~&nbsp;Verpfl.&nbsp;';
		$_preview_values[1]['pnacht']='^class_rhd" nowrap align="center|~&nbsp;Übern.&nbsp;';
		$_preview_values[1]['sumtag']='^class_rhd" nowrap align="center|~&nbsp;Summe&nbsp;';
		$_preview_values[1]['pfrueh']='^class_rhd" nowrap align="center|~&nbsp;Frühst.&nbsp;';
		$_preview_values[1]['pmittag']='^class_rhd" nowrap align="center|~&nbsp;Mittag&nbsp;';
		$_preview_values[1]['pabend']='^class_rhd" nowrap align="center|~&nbsp;Abend&nbsp;';
		$_preview_values[1]['sumist']='^class_rhd" nowrap align="center|~&nbsp;Summe&nbsp;';

		$_preview_values[3]['tage']='^col_e8e8e8" nowrap colspan="4" align="left"><!--|~&nbsp;<u><b>Inlandsreise</b>&nbsp;[123]:</u>&nbsp;<br>&nbsp;Kraftwagen (100&nbsp;km)&nbsp;';
		$_preview_values[3]['stunden']='~-->&nbsp;<u><b>Inlandsreise</b>&nbsp;[123]:</u>&nbsp;<br>&nbsp;Kraftwagen (100&nbsp;km)&nbsp;';
		$_preview_values[3]['pverpflegung']='^col_e8e8e8" nowrap colspan="3" align="left" style="border-left:1px solid #bbbbcc;"><!--|~&nbsp;<b>Zeitraum:</b>&nbsp;<br>&nbsp;24.07.2013, 08:00&nbsp;-&nbsp;26.07.2013, 17:00&nbsp;';
		$_preview_values[3]['sumtag']='~-->&nbsp;<b>Zeitraum:</b>&nbsp;<br>&nbsp;24.07.2013, 08:00&nbsp;-&nbsp;26.07.2013, 17:00&nbsp;';
		$_preview_values[3]['pfrueh']='^col_e8e8e8" nowrap colspan="4" align="left" style="border-left:1px solid #bbbbcc;"><!--|~&nbsp;<b>Startort&nbsp;-&gt;&nbsp;Zielort:</b>&nbsp;<br>&nbsp;Hier&nbsp;-&gt;&nbsp;Dort&nbsp;';
		$_preview_values[3]['sumist']='~-->&nbsp;<b>Startort&nbsp;-&gt;&nbsp;Zielort:</b>&nbsp;<br>&nbsp;Hier&nbsp;-&gt;&nbsp;Dort&nbsp;';

		$_preview_values[4]['tage']='~&nbsp;Mi';
		$_preview_values[4]['datum']='~&nbsp;24.07.2013&nbsp;';
		$_preview_values[4]['stunden']='~&nbsp;16,00&nbsp;h&nbsp;';
		$_preview_values[4]['sumtag']='~&nbsp;12,00&nbsp;&nbsp;';
		$_preview_values[4]['sumist']='~&nbsp;1,50&nbsp;&nbsp;';

		$_preview_values[5]['tage']='~&nbsp;Do';
		$_preview_values[5]['datum']='~&nbsp;25.07.2013&nbsp;';
		$_preview_values[5]['stunden']='~&nbsp;24,00&nbsp;h&nbsp;';
		$_preview_values[5]['sumtag']='~&nbsp;24,00&nbsp;&nbsp;';
		$_preview_values[5]['sumist']='~&nbsp;0,00&nbsp;&nbsp;';

		$_preview_values[6]['tage']='~&nbsp;Fr';
		$_preview_values[6]['datum']='~&nbsp;26.07.2013&nbsp;';
		$_preview_values[6]['stunden']='~&nbsp;17,00&nbsp;h&nbsp;';
		$_preview_values[6]['sumtag']='~&nbsp;12,00&nbsp;&nbsp;';
		$_preview_values[6]['sumist']='~&nbsp;0,00&nbsp;&nbsp;';

		$_preview_values[7]['pverpflegung']='^class_bcr4" nowrap align="right" colspan="2"><!--|~&nbsp;<b>Tagespauschalen:</b>&nbsp;';
		$_preview_values[7]['pnacht']='~-->&nbsp;<b>Tagespauschalen:</b>&nbsp;';
		$_preview_values[7]['sumtag']='^class_bcr4|~&nbsp;<b>48,00</b>&nbsp;&nbsp;';
		$_preview_values[7]['pfrueh']='^class_bcr4" nowrap align="right" colspan="3" style="border-left:1px solid #bbbbcc;"><!--|~&nbsp;<b>abzgl. Sachbezüge:</b>&nbsp;';
		$_preview_values[7]['pabend']='~-->&nbsp;<b>abzgl. Sachbezüge:</b>&nbsp;';
		$_preview_values[7]['sumist']='^class_bcr4|~&nbsp;<b>1,50</b>&nbsp;&nbsp;';

		$_preview_values[8]['tage']='^class_bcr0" nowrap align="left" colspan="10"><!--|~<b>Zwischensumme</b>';
		$_preview_values[8]['pabend']='~--><b>Zwischensumme</b>';
		$_preview_values[8]['sumist']='^class_bcr0|~&nbsp;<b>46,50</b>&nbsp;&nbsp;';

		$_preview_values[10]['tage']='^class_bcr3" nowrap align="left" colspan="10"><!--|~<b>Gesamtsumme</b>';
		$_preview_values[10]['pabend']='~--><b>Gesamtsumme</b>';
		$_preview_values[10]['sumist']='^class_bcr3|~&nbsp;<b>46,50</b>&nbsp;&nbsp;';
	}

	$pdf_noheadlines	= true;
	$pdf_noborders		= true;
	$_preview_tr		= 12;
	$_preview_mask		= 2;

	$incfn='d_manager';
}
else	// Standardreport
{
	// English
	$_preview_header[0]		= '&nbsp;Project&nbsp;';
	$_preview_header[1]		= '&nbsp;Proj.Type&nbsp;';
	$_preview_header[2]		= '&nbsp;Status&nbsp;';
	$_preview_header[16]	= '&nbsp;Pln. Start&nbsp;';
	$_preview_header[17]	= '&nbsp;Pln. End&nbsp;';
	$_preview_header[22]	= '&nbsp;Pln. Effort&nbsp;';
	$_preview_header[23]	= '&nbsp;Act. Effort&nbsp;';
	$_preview_header[24]	= '&nbsp;Rem. Effort&nbsp;';

	$_preview_madd[1][10]		= 0;		// Name_Projekt
	$_preview_madd[1][12]		= 1;		// Projektart
	$_preview_madd[1][14]		= 2;		// Projekt_Status
	$_preview_madd[1][16]		= 'blank';
	$_preview_madd[1][17]		= 'blank';
	$_preview_madd[1][18]		= 'blank';
	$_preview_madd[1][20]		= 'blank';
	$_preview_madd[1][22]		= 'blank';
	$_preview_madd[1][24]		= 'blank';
	$_preview_madd[1][26]		= 'blank';
	$_preview_madd[1][28]		= 'blank';
	$_preview_madd[1][30]		= 'blank';
	$_preview_madd[1][32]		= 'blank';
	$_preview_madd[1][34]		= 'blank';
	$_preview_madd[1][40]		= 'blank';
	$_preview_madd[1][42]		= 'blank';
	$_preview_madd[1][50]		= 16;		// Soll_Beg_Dtm
	$_preview_madd[1][51]		= 17;		// Soll_End_Dtm
	$_preview_madd[1][52]		= 'blank';
	$_preview_madd[1][55]		= 'blank';
	$_preview_madd[1][57]		= 'blank';
	$_preview_madd[1][58]		= 'blank';
	$_preview_madd[1][60]		= 22;		// Aufwand_Soll
	$_preview_madd[1][62]		= 23;		// Aufwand_Ist
	$_preview_madd[1][64]		= 24;		// PufferAufwand
	$_preview_madd[1][66]		= 'blank';
	$_preview_madd[1][70]		= 'blank';
	$_preview_madd[1][72]		= 'blank';
	$_preview_madd[1][75]		= 'blank';
	$_preview_madd[1][76]		= 'blank';
	$_preview_madd[1][77]		= 'blank';
	$_preview_madd[1][78]		= 'blank';
	$_preview_madd[1][80]		= 'blank';
	$_preview_madd[1][85]		= 'blank';
	$_preview_madd[1][87]		= 'blank';
	$_preview_madd[1][89]		= 'blank';
	$_preview_madd[1][90]		= 'blank';
	$_preview_madd[1][91]		= 'blank';
	$_preview_madd[1][92]		= 'blank';
	$_preview_madd[1][93]		= 'blank';
	$_preview_madd[1][94]		= 'blank';
	$_preview_madd[1][96]		= 'blank';
	$_preview_madd[1][98]		= 'blank';
	$_preview_madd[1][100]		= 'blank';
	$_preview_madd[1][101]		= 'blank';
	$_preview_madd[1][102]		= 'blank';
	$_preview_madd[1][103]		= 'blank';
	$_preview_madd[1][104]		= 'blank';
	$_preview_madd[1][109]		= 'blank';
	$_preview_madd[1][110]		= 'blank';
	$_preview_madd[1][111]		= 'blank';
	$_preview_madd[1][112]		= 'blank';
	$_preview_madd[1][113]		= 'blank';
	$_preview_madd[1][114]		= 'blank';
	$_preview_madd[1][119]		= 'blank';
	$_preview_madd[1][120]		= 'blank';
	$_preview_madd[1][121]		= 'blank';
	$_preview_madd[1][129]		= 'blank';
	$_preview_madd[1][130]		= 'blank';
	$_preview_madd[1][131]		= 'blank';
	$_preview_madd[1][132]		= 'blank';
	$_preview_madd[1][133]		= 'blank';
	$_preview_madd[1][134]		= 'blank';
	$_preview_madd[1][135]		= 'blank';
	$_preview_madd[1][136]		= 'blank';
	$_preview_madd[1][138]		= 'blank';
	$_preview_madd[1][140]		= 'blank';
	$_preview_madd[1][141]		= 'blank';
	$_preview_madd[1][142]		= 'blank';
	$_preview_madd[1][143]		= 'blank';
	$_preview_madd[1][144]		= 'blank';
	$_preview_madd[1][145]		= 'blank';
	$_preview_madd[1][149]		= 'blank';
	$_preview_madd[1][150]		= 'blank';
	$_preview_madd[1][151]		= 'blank';
	$_preview_madd[1][152]		= 'blank';
	$_preview_madd[1][153]		= 'blank';
	$_preview_madd[1][154]		= 'blank';
	$_preview_madd[1][155]		= 'blank';
	$_preview_madd[1][157]		= 'blank';
	$_preview_madd[1][158]		= 'blank';
	$_preview_madd[1][159]		= 'blank';
	$_preview_madd[1][160]		= 'blank';
	$_preview_madd[1][161]		= 'blank';
	$_preview_madd[1][162]		= 'blank';
	$_preview_madd[1][163]		= 'blank';
	$_preview_madd[1][169]		= 'blank';
	$_preview_madd[1][170]		= 'blank';
	$_preview_madd[1][171]		= 'blank';
	$_preview_madd[1][172]		= 'blank';
	$_preview_madd[1][173]		= 'blank';
	$_preview_madd[1][179]		= 'blank';
	$_preview_madd[1][180]		= 'blank';

	$_preview_madd['lines']		= 1;
	$_preview_madd['columns']	= '10,12,14,50,51,60,62,64';

	$_preview_madd['rep_headline']			= 'Standard Report Test';

	$_preview_values[0]['name_projekt']		= '^col_e8e8e8|~&nbsp;<font color="#000000"><b>50</b> - MP</font>&nbsp;';
	$_preview_values[0]['projektart']		= '^col_e8e8e8|~&nbsp;Motherproject&nbsp;';
	$_preview_values[0]['projekt_status']	= '^col_e8e8e8|~&nbsp;active&nbsp;';
	$_preview_values[0]['soll_beg_dtm']		= '^col_e8e8e8|~&nbsp;08/05/2013&nbsp;';
	$_preview_values[0]['soll_end_dtm']		= '^col_e8e8e8|~&nbsp;08/30/2013&nbsp;';
	$_preview_values[0]['aufwand_soll']		= '^col_e8e8e8|~&nbsp;<font class="black">200:00</font>&nbsp;h&nbsp;';
	$_preview_values[0]['aufwand_ist']		= '^col_e8e8e8|~&nbsp;<font class="black">31:15</font>&nbsp;h&nbsp;';
	$_preview_values[0]['pufferaufwand']	= '^col_e8e8e8|~&nbsp;<font class="black">168:45</font>&nbsp;h&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

	$_preview_values[1]['name_projekt']		= '^col_ececec|~&nbsp;&nbsp;&nbsp;&nbsp;<font color="#000000"><b>50.1</b> - SP1</font>&nbsp;';
	$_preview_values[1]['projektart']		= '^col_ececec|~&nbsp;Subproject&nbsp;';
	$_preview_values[1]['projekt_status']	= '^col_ececec|~&nbsp;active&nbsp;';
	$_preview_values[1]['soll_beg_dtm']		= '^col_ececec|~&nbsp;08/05/2013&nbsp;';
	$_preview_values[1]['soll_end_dtm']		= '^col_ececec|~&nbsp;08/23/2013&nbsp;';
	$_preview_values[1]['aufwand_soll']		= '^col_ececec|~&nbsp;<font class="black">140:00</font>&nbsp;h&nbsp;';
	$_preview_values[1]['aufwand_ist']		= '^col_ececec|~&nbsp;<font class="black">31:15</font>&nbsp;h&nbsp;';
	$_preview_values[1]['pufferaufwand']	= '^col_ececec|~&nbsp;<font class="black">108:45</font>&nbsp;h&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

	$_preview_values[2]['name_projekt']		= '^col_fcfcfc|~&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#000000"><b>50.1.1</b> - T1-1</font>&nbsp;';
	$_preview_values[2]['projektart']		= '^col_fcfcfc|~&nbsp;Task&nbsp;';
	$_preview_values[2]['projekt_status']	= '^col_fcfcfc|~&nbsp;active&nbsp;';
	$_preview_values[2]['soll_beg_dtm']		= '^col_fcfcfc|~&nbsp;08/05/2013&nbsp;';
	$_preview_values[2]['soll_end_dtm']		= '^col_fcfcfc|~&nbsp;08/16/2013&nbsp;';
	$_preview_values[2]['aufwand_soll']		= '^col_fcfcfc|~&nbsp;<font class="black">100:00</font>&nbsp;h&nbsp;';
	$_preview_values[2]['aufwand_ist']		= '^col_fcfcfc|~&nbsp;<font class="black">31:15</font>&nbsp;h&nbsp;';
	$_preview_values[2]['pufferaufwand']	= '^col_fcfcfc|~&nbsp;<font class="black">68:45</font>&nbsp;h&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

	$_preview_values[3]['name_projekt']		= '^col_ffffff|~&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#000000"><b>50.1.2</b> - T1-2</font>&nbsp;';
	$_preview_values[3]['projektart']		= '^col_ffffff|~&nbsp;Task&nbsp;';
	$_preview_values[3]['projekt_status']	= '^col_ffffff|~&nbsp;planned&nbsp;';
	$_preview_values[3]['soll_beg_dtm']		= '^col_ffffff|~&nbsp;08/12/2013&nbsp;';
	$_preview_values[3]['soll_end_dtm']		= '^col_ffffff|~&nbsp;08/23/2013&nbsp;';
	$_preview_values[3]['aufwand_soll']		= '^col_ffffff|~&nbsp;<font class="black">40:00</font>&nbsp;h&nbsp;';
	$_preview_values[3]['aufwand_ist']		= '^col_ffffff|~&nbsp;<font class="black">0:00</font>&nbsp;h&nbsp;';
	$_preview_values[3]['pufferaufwand']	= '^col_ffffff|~&nbsp;<font class="black">40:00</font>&nbsp;h&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

	$_preview_values[4]['name_projekt']		= '^col_e8e8e8|~&nbsp;&nbsp;&nbsp;&nbsp;<font color="#000000"><b>50.2</b> - SP2</font>&nbsp;';
	$_preview_values[4]['projektart']		= '^col_e8e8e8|~&nbsp;Subproject&nbsp;';
	$_preview_values[4]['projekt_status']	= '^col_e8e8e8|~&nbsp;planned&nbsp;';
	$_preview_values[4]['soll_beg_dtm']		= '^col_e8e8e8|~&nbsp;08/26/2013&nbsp;';
	$_preview_values[4]['soll_end_dtm']		= '^col_e8e8e8|~&nbsp;08/30/2013&nbsp;';
	$_preview_values[4]['aufwand_soll']		= '^col_e8e8e8|~&nbsp;<font class="black">60:00</font>&nbsp;h&nbsp;';
	$_preview_values[4]['aufwand_ist']		= '^col_e8e8e8|~&nbsp;<font class="black">0:00</font>&nbsp;h&nbsp;';
	$_preview_values[4]['pufferaufwand']	= '^col_e8e8e8|~&nbsp;<font class="black">60:00</font>&nbsp;h&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

	$_preview_values[5]['name_projekt']		= '^col_ffffff|~&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#000000"><b>50.2.1</b> - T2</font>&nbsp;';
	$_preview_values[5]['projektart']		= '^col_ffffff|~&nbsp;Task&nbsp;';
	$_preview_values[5]['projekt_status']	= '^col_ffffff|~&nbsp;planned&nbsp;';
	$_preview_values[5]['soll_beg_dtm']		= '^col_ffffff|~&nbsp;08/26/2013&nbsp;';
	$_preview_values[5]['soll_end_dtm']		= '^col_ffffff|~&nbsp;08/30/2013&nbsp;';
	$_preview_values[5]['aufwand_soll']		= '^col_ffffff|~&nbsp;<font class="black">60:00</font>&nbsp;h&nbsp;';
	$_preview_values[5]['aufwand_ist']		= '^col_ffffff|~&nbsp;<font class="black">0:00</font>&nbsp;h&nbsp;';
	$_preview_values[5]['pufferaufwand']	= '^col_ffffff|~&nbsp;<font class="black">60:00</font>&nbsp;h&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

	$_preview_values[6]['name_projekt']		= '^class_mn1|~&nbsp;Grand-total&nbsp;';
	$_preview_values[6]['projektart']		= '^class_mn0|~&nbsp;';
	$_preview_values[6]['projekt_status']	= '^class_mn0|~&nbsp;';
	$_preview_values[6]['soll_beg_dtm']		= '^class_mn0|~&nbsp;';
	$_preview_values[6]['soll_end_dtm']		= '^class_mn0|~&nbsp;';
	$_preview_values[6]['aufwand_soll']		= '^class_mn1|~&nbsp;200:00 h&nbsp;';
	$_preview_values[6]['aufwand_ist']		= '^class_mn1|~&nbsp;31:15 h&nbsp;';
	$_preview_values[6]['pufferaufwand']	= '^class_mn1|~&nbsp;168:45&nbsp;h&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

	// Deutsch
	if($lng == 1)
	{
		$_preview_header[0]		= '&nbsp;Projekt&nbsp;';
		$_preview_header[1]		= '&nbsp;Proj.Art&nbsp;';
		$_preview_header[2]		= '&nbsp;Status&nbsp;';
		$_preview_header[16]	= '&nbsp;Gepl. Start&nbsp;';
		$_preview_header[17]	= '&nbsp;Gepl. Ende&nbsp;';
		$_preview_header[22]	= '&nbsp;Aufwand Soll&nbsp;';
		$_preview_header[23]	= '&nbsp;Aufwand Ist&nbsp;';
		$_preview_header[24]	= '&nbsp;Restaufwand&nbsp;';

		$_preview_madd['rep_headline']			= 'Standardreport Test';

		$_preview_values[0]['name_projekt']		= '^col_e8e8e8|~&nbsp;<font color="#000000"><b>50</b> - MP</font>&nbsp;';
		$_preview_values[0]['projektart']		= '^col_e8e8e8|~&nbsp;Mutterprojekt&nbsp;';
		$_preview_values[0]['projekt_status']	= '^col_e8e8e8|~&nbsp;aktiv&nbsp;';
		$_preview_values[0]['soll_beg_dtm']		= '^col_e8e8e8|~&nbsp;05.08.2013&nbsp;';
		$_preview_values[0]['soll_end_dtm']		= '^col_e8e8e8|~&nbsp;30.08.2013&nbsp;';
		$_preview_values[0]['aufwand_soll']		= '^col_e8e8e8|~&nbsp;<font class="black">200:00</font>&nbsp;Std.&nbsp;';
		$_preview_values[0]['aufwand_ist']		= '^col_e8e8e8|~&nbsp;<font class="black">31:15</font>&nbsp;Std.&nbsp;';
		$_preview_values[0]['pufferaufwand']	= '^col_e8e8e8|~&nbsp;<font class="black">168:45</font>&nbsp;Std.&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

		$_preview_values[1]['name_projekt']		= '^col_ececec|~&nbsp;&nbsp;&nbsp;&nbsp;<font color="#000000"><b>50.1</b> - TP1</font>&nbsp;';
		$_preview_values[1]['projektart']		= '^col_ececec|~&nbsp;Teilprojekt&nbsp;';
		$_preview_values[1]['projekt_status']	= '^col_ececec|~&nbsp;aktiv&nbsp;';
		$_preview_values[1]['soll_beg_dtm']		= '^col_ececec|~&nbsp;05.08.2013&nbsp;';
		$_preview_values[1]['soll_end_dtm']		= '^col_ececec|~&nbsp;23.08.2013&nbsp;';
		$_preview_values[1]['aufwand_soll']		= '^col_ececec|~&nbsp;<font class="black">140:00</font>&nbsp;Std.&nbsp;';
		$_preview_values[1]['aufwand_ist']		= '^col_ececec|~&nbsp;<font class="black">31:15</font>&nbsp;Std.&nbsp;';
		$_preview_values[1]['pufferaufwand']	= '^col_ececec|~&nbsp;<font class="black">108:45</font>&nbsp;Std.&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

		$_preview_values[2]['name_projekt']		= '^col_fcfcfc|~&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#000000"><b>50.1.1</b> - A1-1</font>&nbsp;';
		$_preview_values[2]['projektart']		= '^col_fcfcfc|~&nbsp;Aufgabe&nbsp;';
		$_preview_values[2]['projekt_status']	= '^col_fcfcfc|~&nbsp;aktiv&nbsp;';
		$_preview_values[2]['soll_beg_dtm']		= '^col_fcfcfc|~&nbsp;05.08.2013&nbsp;';
		$_preview_values[2]['soll_end_dtm']		= '^col_fcfcfc|~&nbsp;16.08.2013&nbsp;';
		$_preview_values[2]['aufwand_soll']		= '^col_fcfcfc|~&nbsp;<font class="black">100:00</font>&nbsp;Std.&nbsp;';
		$_preview_values[2]['aufwand_ist']		= '^col_fcfcfc|~&nbsp;<font class="black">31:15</font>&nbsp;Std.&nbsp;';
		$_preview_values[2]['pufferaufwand']	= '^col_fcfcfc|~&nbsp;<font class="black">68:45</font>&nbsp;Std.&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

		$_preview_values[3]['name_projekt']		= '^col_ffffff|~&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#000000"><b>50.1.2</b> - A1-2</font>&nbsp;';
		$_preview_values[3]['projektart']		= '^col_ffffff|~&nbsp;Aufgabe&nbsp;';
		$_preview_values[3]['projekt_status']	= '^col_ffffff|~&nbsp;geplant&nbsp;';
		$_preview_values[3]['soll_beg_dtm']		= '^col_ffffff|~&nbsp;12.08.2013&nbsp;';
		$_preview_values[3]['soll_end_dtm']		= '^col_ffffff|~&nbsp;23.08.2013&nbsp;';
		$_preview_values[3]['aufwand_soll']		= '^col_ffffff|~&nbsp;<font class="black">40:00</font>&nbsp;Std.&nbsp;';
		$_preview_values[3]['aufwand_ist']		= '^col_ffffff|~&nbsp;<font class="black">0:00</font>&nbsp;Std.&nbsp;';
		$_preview_values[3]['pufferaufwand']	= '^col_ffffff|~&nbsp;<font class="black">40:00</font>&nbsp;Std.&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

		$_preview_values[4]['name_projekt']		= '^col_e8e8e8|~&nbsp;&nbsp;&nbsp;&nbsp;<font color="#000000"><b>50.2</b> - TP2</font>&nbsp;';
		$_preview_values[4]['projektart']		= '^col_e8e8e8|~&nbsp;Teilprojekt&nbsp;';
		$_preview_values[4]['projekt_status']	= '^col_e8e8e8|~&nbsp;geplant&nbsp;';
		$_preview_values[4]['soll_beg_dtm']		= '^col_e8e8e8|~&nbsp;26.08.2013&nbsp;';
		$_preview_values[4]['soll_end_dtm']		= '^col_e8e8e8|~&nbsp;30.08.2013&nbsp;';
		$_preview_values[4]['aufwand_soll']		= '^col_e8e8e8|~&nbsp;<font class="black">60:00</font>&nbsp;Std.&nbsp;';
		$_preview_values[4]['aufwand_ist']		= '^col_e8e8e8|~&nbsp;<font class="black">0:00</font>&nbsp;Std.&nbsp;';
		$_preview_values[4]['pufferaufwand']	= '^col_e8e8e8|~&nbsp;<font class="black">60:00</font>&nbsp;Std.&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

		$_preview_values[5]['name_projekt']		= '^col_ffffff|~&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color="#000000"><b>50.2.1</b> - A2</font>&nbsp;';
		$_preview_values[5]['projektart']		= '^col_ffffff|~&nbsp;Aufgabe&nbsp;';
		$_preview_values[5]['projekt_status']	= '^col_ffffff|~&nbsp;geplant&nbsp;';
		$_preview_values[5]['soll_beg_dtm']		= '^col_ffffff|~&nbsp;26.08.2013&nbsp;';
		$_preview_values[5]['soll_end_dtm']		= '^col_ffffff|~&nbsp;30.08.2013&nbsp;';
		$_preview_values[5]['aufwand_soll']		= '^col_ffffff|~&nbsp;<font class="black">60:00</font>&nbsp;Std.&nbsp;';
		$_preview_values[5]['aufwand_ist']		= '^col_ffffff|~&nbsp;<font class="black">0:00</font>&nbsp;Std.&nbsp;';
		$_preview_values[5]['pufferaufwand']	= '^col_ffffff|~&nbsp;<font class="black">60:00</font>&nbsp;Std.&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';

		$_preview_values[6]['name_projekt']		= '^class_mn1|~&nbsp;Gesamtsumme&nbsp;';
		$_preview_values[6]['projektart']		= '^class_mn0|~&nbsp;';
		$_preview_values[6]['projekt_status']	= '^class_mn0|~&nbsp;';
		$_preview_values[6]['soll_beg_dtm']		= '^class_mn0|~&nbsp;';
		$_preview_values[6]['soll_end_dtm']		= '^class_mn0|~&nbsp;';
		$_preview_values[6]['aufwand_soll']		= '^class_mn1|~&nbsp;200:00 Std.&nbsp;';
		$_preview_values[6]['aufwand_ist']		= '^class_mn1|~&nbsp;31:15 Std.&nbsp;';
		$_preview_values[6]['pufferaufwand']	= '^class_mn1|~&nbsp;168:45&nbsp;Std.&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'green_10.png" width="10" height="10">&nbsp;';
	}

	$_preview_tr		= 152;
	$_preview_mask		= 349;
	$_preview_headline	= $_preview_madd['rep_headline'];

	$incfn='d_manager';
}

// Include file manager?
if(strlen($incfn))
{
//fxDebug($_preview_values,'$_preview_values: $incfn='.$incfn, 0);
	$GLOBALS['__includer'][$incfn]=true;
	require('includer.inc');
}
?>