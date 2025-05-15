<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : config_preview.inc                                           //
// Version     : 24.1                                                         //
// Begin       : 2020-10-15                                                   //
// Last Change : 2024-03-14                                                   //
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
 * @version 24.1
 */

if(isset($_GET) && isset($_GET['pf']) && strlen((string)$_GET['pf']))
{
	$filename=urldecode('./DAT/TMP/~'.trim((string)$_GET['pf']).'.pdf');
	if(file_exists($filename))
	{
		// Disable output compression, or else the filesize doesn't match
		ini_set('zlib.output_compression', 'Off');

		$len=filesize($filename);

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

//fxDebug('_GET'); fxDebug('_POST');

setForm(true);
fxSession('t','l');

// Language
$lng=0;
if(!$lng && fxIsArray($_POST) && isset($_POST['lng']))
	$lng=(int)$_POST['lng'];
if(!$lng)
	$lng=$GLOBALS['fxpglobals']['lang'];
$lng_backup=$GLOBALS['fxpglobals']['lang'];
$GLOBALS['fxpglobals']['lang']=$lng;

// Submitted configuration preview type and text
$_preview_type=0;
if(fxIsArray($_GET) && isset($_GET['ctype']))
	$_preview_type=(int)$_GET['ctype'];
$_preview_config=false;
$ctext='';
if(fxIsArray($_POST) && isset($_POST['ctext']))
{
	$ctext=urldecode($_POST['ctext']);
	$nltr=array("\r\n"=>'<:nl:>', "\r"=>'<:nl:>', "\n"=>'<:nl:>');
	$_preview_config=explode('<:nl:>', strtr($ctext, $nltr));
}
//fxDebug($_preview_config,'$_preview_config: $_preview_type='.$_preview_type.', $lng='.$lng, 0);

// Include filename depending on preview type
$incfn='d_manager';

// Preview substitutions
$units=getUnits();
//fxDebug($units,'$units', 0);
$year=substr((string)$GLOBALS['date'],0,4);
$month=substr((string)$GLOBALS['date'],4,2);
$txt_preview=meldung(3387,false);	// Preview
$txt_project=meldung(585,false);	// Project
$txt_subproject=meldung(582,false);	// Sub-project
$txt_task=meldung(621,false);		// Task
$txt_cw=meldung(1036,false);		// CW
// ...Deutsch
if($lng == 1)
	$_preview_subst=array('name'=>'Mustermann, Max', 'sex'=>'Herr', 'name2'=>'Musterfrau, Maria', 'sex2'=>'Frau', 'path'=>'', 'pnumber'=>69, 'pname'=>'Hauptprojekt', 'ppnumber'=>69, 'ppname'=>'Hauptprojekt', 'ppath'=>'69 Hauptprojekt', 'project'=>'69 Hauptprojekt', 'timespan'=>'01.'.$month.'.'.$year.' - 28.'.$month.'.'.$year);
// ... English
else
	$_preview_subst=array('name'=>'Doe, John', 'sex'=>'Mr', 'name2'=>'Doe, Jane', 'sex2'=>'Mrs', 'path'=>'', 'pnumber'=>69, 'pname'=>'Main Project', 'ppnumber'=>69, 'ppname'=>'Main Project', 'ppath'=>'69 Main Project', 'project'=>'69 Main Project', 'timespan'=>$month.'/01/'.$year.' - '.$month.'/28/'.$year);
$_preview_subst['timespan']=get_entry($year.$month.'01', 'date').' - '.get_entry($year.$month.'28', 'date');
$rna=array($_preview_subst['name'],$_preview_subst['name2'], $_preview_subst['sex'].' '.$_preview_subst['name'],$_preview_subst['sex2'].' '.$_preview_subst['name2']);

// Lorem ipsum sentences
$lia=array(
	'Lorem ipsum dolor sit amet, consectetuer sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.',
	'At vero eos et accusam et justo duo dolores et ea rebum.',
	'Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.',
	'Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis.',
	'At vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.',
	'Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.'
);

////////////////////////////////////////////////////////////////////////////////
// PREVIEWS                                                                   //
////////////////////////////////////////////////////////////////////////////////
// ...Email (HTML)
if($_preview_type == 2018)
{
	// We need person functions
	$GLOBALS['__includer']['tools_pers']=true;
	require('includer.inc');

	// Generate random text
	$etxt='';
	for($p=0; $p<random_int(3,7); $p++)
	{
		$ptxt='';
		for($t=0; $t<random_int(3,6); $t++)
		{
			if(strlen((string)$ptxt))
				$ptxt .= ' ';
			$ptxt .= $lia[random_int(0,5)];
		}

		if(strlen((string)$etxt))
			$etxt .= '<br /><br />';
		$etxt .= $ptxt;
	}

	// Generate email
	$preview=send_mail('fxp@fx-project.org', 'Email '.$txt_preview, '<b>Lorem ipsum</b><br><br><br>'.$etxt, 0, 0, false, true);

	$GLOBALS['fxpglobals']['lang']=$lng_backup;
	die($preview);
}

// ...Graphic Charts - Vacation Plan/Gantt etc. (PDF)
else if($_preview_type == 2020)
{
	$_preview_cal=array();

	// Line 0: Project-Status
	$_preview_cal[0]=array(
		0=>array('text'=>'&nbsp;<b>'.meldung(1102,false).'</b>:&nbsp;', 'color'=>'ffffff', 'typ'=>'All', 'title'=>$txt_preview.': '.$_preview_subst['project']),
		1=>array('text'=>'&nbsp;'.$_preview_subst['project'].'&nbsp;', 'colspan'=>3, 'color'=>'f8f8f8', 'typ'=>'All'),
		4=>array('text'=>'-', 'colspan'=>1, 'align'=>'center', 'color'=>'d8d8d8', 'font'=>'color="#787878"'),
		5=>array('text'=>meldung((int)$month+17,false).' '.$year, 'colspan'=>28, 'color'=>'dfdfdf', 'font'=>'color="#787878"'),
		33=>array('text'=>'+', 'colspan'=>1, 'align'=>'center', 'color'=>'d8d8d8', 'font'=>'color="#787878"')
	);

	// Line 1: Time Span
	$cw=kw($year.$month.'01');
	$_preview_cal[1]=array(
		0=>array('text'=>'&nbsp;<b>'.meldung(1105,false).'</b>:&nbsp;', 'color'=>'ffffff', 'typ'=>'All'),
		1=>array('text'=>'&nbsp;'.$_preview_subst['timespan'].'&nbsp;', 'colspan'=>3, 'color'=>'f8f8f8', 'typ'=>'All'),
		4=>array('text'=>'-', 'colspan'=>1, 'align'=>'center', 'color'=>'e8e8e8', 'font'=>'color="#787878"'),
		5=>array('text'=>$txt_cw.' '.substr('0'.$cw,-2), 'colspan'=>7, 'color'=>'e8e8e8', 'font'=>'color="#787878"'),
		12=>array('text'=>$txt_cw.' '.substr('0'.($cw+1),-2), 'colspan'=>7, 'color'=>'efefef', 'font'=>'color="#787878"'),
		19=>array('text'=>$txt_cw.' '.substr('0'.($cw+2),-2), 'colspan'=>7, 'color'=>'e8e8e8', 'font'=>'color="#787878"'),
		26=>array('text'=>$txt_cw.' '.substr('0'.($cw+3),-2), 'colspan'=>7, 'color'=>'efefef', 'font'=>'color="#787878"'),
		33=>array('text'=>'+', 'colspan'=>1, 'align'=>'center', 'color'=>'efefef', 'font'=>'color="#787878"')
	);

	// Line 2: Project Name + Project Duration
	$_preview_cal[2]=array(
		0=>array('text'=>'&nbsp;<b>'.meldung(1103,false).'</b>:&nbsp;', 'color'=>'dddddd', 'typ'=>'All'),
		1=>array('text'=>'&nbsp;<b>'.meldung(1104,false).'</b>:&nbsp;', 'color'=>'dddddd', 'typ'=>'All'),
		2=>array('colspan'=>2)
	);
	$bc='ffffff';
	for($d=0; $d<30; $d++)
	{
		if(!$d)
			$dt='-';
		else if($d > 28)
			$dt='+';
		else
			$dt=substr('0'.$d,-2);
		$_preview_cal[2][4+$d]=array('text'=>'<div style="width:22px;overflow:hidden;">'.$dt.'</div>', 'width'=>22, 'align'=>'center', 'color'=>$bc, 'font'=>'color="#787878"', 'typ'=>'All');
		if($bc == 'f0f0f0')
			$bc='ffffff';
		else
			$bc='f0f0f0';
	}

	// Line 3: Main Project
	$timespan=get_entry($year.$month.'01', 'date').' - '.get_entry($year.$month.'28', 'date');
	$_preview_cal[3]=array(
		0=>array('text'=>'&nbsp;<font class="darkgrey">'.$_preview_subst['pnumber'].'</font>&nbsp;<font class="grey bold">'.$_preview_subst['pname'].'</font>&nbsp;', 'color'=>'f4f4f4', 'typ'=>'All', 'pt'=>'project'),
		1=>array('text'=>'&nbsp;<i class="grey">('.$timespan.')</i>&nbsp;', 'color'=>'f4f4f4', 'align'=>'center', 'typ'=>'All'),
		4=>array('color'=>'ffffff', 'colspan'=>1),
		5=>array('text'=>'<b>'.$_preview_subst['pname'].'</b>', 'colspan'=>28, 'align'=>'left" valign="top', 'bgp'=>'gfxpath', 'background'=>'g_bg_d', 'pkl'=>true, 'pkr'=>true, 'ikl'=>false, 'ikr'=>false, 'anchor'=>true, 'status'=>300, 'mode'=>'010'),
		12=>array('color'=>'ffffff', 'colspan'=>1)
	);

	// Line 4: Sub Project 1
	$timespan=get_entry($year.$month.'01', 'date').' - '.get_entry($year.$month.'14', 'date');
	$_preview_cal[4]=array(
		0=>array('text'=>'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font class="darkgrey">'.$_preview_subst['pnumber'].'.1</font>&nbsp;<font class="grey bold">'.$txt_subproject.' 1</font>&nbsp;', 'color'=>'f8f8f8', 'typ'=>'All', 'pt'=>'project'),
		1=>array('text'=>'&nbsp;<i class="grey">('.$timespan.')</i>&nbsp;', 'color'=>'f8f8f8', 'align'=>'center', 'typ'=>'All'),
		4=>array('color'=>'ffffff', 'colspan'=>1),
		5=>array('text'=>'<b>'.$txt_subproject.' 1</b>', 'colspan'=>14, 'align'=>'left" valign="top', 'bgp'=>'gfxpath', 'background'=>'g_bg_d', 'pkl'=>true, 'pkr'=>true, 'ikl'=>false, 'ikr'=>false, 'anchor'=>true, 'status'=>FXP_PS_ACTIVE, 'mode'=>'010'),
		19=>array('color'=>'ffffff', 'colspan'=>15)
	);

	// Line 5: Task 1.1
	$timespan=get_entry($year.$month.'01', 'date').' - '.get_entry($year.$month.'07', 'date');
	$_preview_cal[5]=array(
		0=>array('text'=>'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font class="black">'.$_preview_subst['pnumber'].'.1.1</font>&nbsp;<font class="darkgrey">'.$txt_task.' 1.1</font>&nbsp;', 'color'=>'f4f4f4', 'typ'=>'All', 'pt'=>'task'),
		1=>array('text'=>'&nbsp;<i class="grey">('.$timespan.')</i>&nbsp;', 'color'=>'f4f4f4', 'align'=>'center', 'typ'=>'All'),
		4=>array('color'=>'ffffff', 'colspan'=>1),
		5=>array('text'=>$txt_task.' 1.1', 'colspan'=>7, 'align'=>'left" valign="top', 'bgp'=>'gfxpath', 'background'=>'g_bg_d', 'pkl'=>true, 'pkr'=>true, 'ikl'=>false, 'ikr'=>false, 'anchor'=>true, 'status'=>FXP_PS_COMPLETED, 'mode'=>'010'),
		12=>array('color'=>'ffffff', 'colspan'=>22)
	);

	// Line 6: Task 1.2
	$timespan=get_entry($year.$month.'08', 'date').' - '.get_entry($year.$month.'14', 'date');
	$_preview_cal[6]=array(
		0=>array('text'=>'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font class="black">'.$_preview_subst['pnumber'].'.1.2</font>&nbsp;<font class="darkgrey">'.$txt_task.' 1.2</font>&nbsp;', 'color'=>'f4f4f4', 'typ'=>'All', 'pt'=>'task'),
		1=>array('text'=>'&nbsp;<i class="grey">('.$timespan.')</i>&nbsp;', 'color'=>'f4f4f4', 'align'=>'center', 'typ'=>'All'),
		4=>array('color'=>'ffffff', 'colspan'=>8),
		12=>array('text'=>$txt_task.' 1.2', 'colspan'=>7, 'align'=>'left" valign="top', 'bgp'=>'gfxpath', 'background'=>'g_bg_d', 'pkl'=>true, 'pkr'=>true, 'ikl'=>false, 'ikr'=>false, 'anchor'=>true, 'status'=>FXP_PS_ACTIVE, 'mode'=>'010'),
		19=>array('color'=>'ffffff', 'colspan'=>15)
	);

	// Line 7: Sub Project 2
	$timespan=get_entry($year.$month.'15', 'date').' - '.get_entry($year.$month.'28', 'date');
	$_preview_cal[7]=array(
		0=>array('text'=>'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font class="darkgrey">'.$_preview_subst['pnumber'].'.2</font>&nbsp;<font class="grey bold">'.$txt_subproject.' 2</font>&nbsp;', 'color'=>'f8f8f8', 'typ'=>'All', 'pt'=>'project'),
		1=>array('text'=>'&nbsp;<i class="grey">('.$timespan.')</i>&nbsp;', 'color'=>'f8f8f8', 'align'=>'center', 'typ'=>'All'),
		4=>array('color'=>'ffffff', 'colspan'=>15),
		19=>array('text'=>'<b>'.$txt_subproject.' 2</b>', 'colspan'=>14, 'align'=>'left" valign="top', 'bgp'=>'gfxpath', 'background'=>'g_bg_d', 'pkl'=>true, 'pkr'=>true, 'ikl'=>false, 'ikr'=>false, 'anchor'=>true, 'status'=>FXP_PS_PLANNED, 'mode'=>'010'),
		33=>array('color'=>'ffffff', 'colspan'=>1)
	);

	// Line 8: Task 2.1
	$timespan=get_entry($year.$month.'15', 'date').' - '.get_entry($year.$month.'21', 'date');
	$_preview_cal[8]=array(
		0=>array('text'=>'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font class="black">'.$_preview_subst['pnumber'].'.2.1</font>&nbsp;<font class="darkgrey">'.$txt_task.' 2.1</font>&nbsp;', 'color'=>'f4f4f4', 'typ'=>'All', 'pt'=>'task'),
		1=>array('text'=>'&nbsp;<i class="grey">('.$timespan.')</i>&nbsp;', 'color'=>'f4f4f4', 'align'=>'center', 'typ'=>'All'),
		4=>array('color'=>'ffffff', 'colspan'=>15),
		19=>array('text'=>$txt_task.' 2.1', 'colspan'=>7, 'align'=>'left" valign="top', 'bgp'=>'gfxpath', 'background'=>'g_bg_d', 'pkl'=>true, 'pkr'=>true, 'ikl'=>false, 'ikr'=>false, 'anchor'=>true, 'status'=>FXP_PS_PLANNED, 'mode'=>'010'),
		26=>array('color'=>'ffffff', 'colspan'=>8)
	);

	// Line 9: Task 2.2
	$timespan=get_entry($year.$month.'22', 'date').' - '.get_entry($year.$month.'28', 'date');
	$_preview_cal[9]=array(
		0=>array('text'=>'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font class="black">'.$_preview_subst['pnumber'].'.2.2</font>&nbsp;<font class="darkgrey">'.$txt_task.' 2.2</font>&nbsp;', 'color'=>'f4f4f4', 'typ'=>'All', 'pt'=>'task'),
		1=>array('text'=>'&nbsp;<i class="grey">('.$timespan.')</i>&nbsp;', 'color'=>'f4f4f4', 'align'=>'center', 'typ'=>'All'),
		4=>array('color'=>'ffffff', 'colspan'=>22),
		26=>array('text'=>$txt_task.' 2.2', 'colspan'=>7, 'align'=>'left" valign="top', 'bgp'=>'gfxpath', 'background'=>'g_bg_d', 'pkl'=>true, 'pkr'=>true, 'ikl'=>false, 'ikr'=>false, 'anchor'=>true, 'status'=>FXP_PS_INACTIVE, 'mode'=>'010'),
		33=>array('color'=>'ffffff', 'colspan'=>1)
	);

	$_preview_tr=26;
	$incfn='d_manager_pst';
}

// ...Invoice (PDF)
else if($_preview_type == 2021)
{
	// Steering parameters
	$sp_intprocess=false;
	$sp_noprocnr=false;
	$sp_novat=false;
	$pdf_array=load_pdfconfig(2);
//fxDebug($pdf_array, $selname, 0);
	if(fxIsArray($pdf_array))
	{
		if(isset($pdf_array['intprocess']))
			$sp_intprocess=true;
		if(isset($pdf_array['noprocnr']))
			$sp_noprocnr=true;
		if(isset($pdf_array['novat']))
			$sp_novat=true;
	}
//$sp_intprocess=false; $sp_noprocnr=false; $sp_novat=false; fxDebug($sp_intprocess,'$sp_intprocess', 0); fxDebug($sp_noprocnr,'$sp_noprocnr', 0); fxDebug($sp_novat,'$sp_novat', 0);

	// Header literals: Pos., Project, Description, Amount, Unit, VAT, UP Net, TP Net, TP VAT, TP Gross
	$_preview_header=array(0=>get_text(1049,'5'), get_text(46,'10'), get_text(510,'15'), get_text(1065,'10'), get_text(1056,'10'), get_text(71,'5'), get_text(1064,'10'), get_text(1949,'10'), get_text(1952,'10'), get_text(1953,'10'));
//fxDebug($_preview_header,'$_preview_header', 0);

	// Mask definitions
	$_preview_madd=array(
		1=>array(1=>0,1,2,3,4,5,6,7,8,9),
		'lines'=>1,
		'columns'=>'',
		'rep_headline'=>meldung(872,false).': '.$txt_preview	// Invoice: Preview
	);
	// Hide columns?
	$cspan=8;
	$clast='r_gesamtbrutto';
	if($sp_intprocess)	// Hide: Project
		$_preview_madd[1][2]='blank';
	if($sp_novat)		// Hide: VAT, TP VAT + TP Gross
	{
		$_preview_madd[1][6]='blank';
		$_preview_madd[1][9]='blank';
		$_preview_madd[1][10]='blank';
		$cspan -= 3;
		$clast='r_gesamtpreis';
	}
	foreach($_preview_madd[1] as $mk => $mv)
	{
		if($mv !== 'blank')
		{
			if(strlen((string)$_preview_madd['columns']))
				$_preview_madd['columns'] .= ',';
			$_preview_madd['columns'] .= $mk;
		}
	}
//fxDebug($_preview_madd,'$_preview_madd', 0);

	if($lng == 1)	// Deutsch
		$txt_notes='(Lange optionale Zeile für Anmerkungen)';
	else			// English
		$txt_notes='(Long optional line for notes)';

	// Preview values
	$ln=0;
	$rv=1;
	$spn=0.0;
	$spv=0.0;
	$spb=0.0;
	$vat=get_double($GLOBALS['fxpglobals']['settings']['mwst_satz']);
	$_preview_values=array();
	for($p=1; $p<random_int(4,8); $p++)
	{
		$rcl='^class_rv'.($rv%2).'|';
		$tsk=$txt_task.' '.$p;
		if(!$sp_noprocnr)
			$tsk=$_preview_subst['pnumber'].'.'.$p.' - '.$tsk;
		$amt=random_int(1,100);
		$upn=70+random_int(0,5)*5;
		$tpn=$upn*$amt;
		$tpg=$tpn*(1 + ($vat/100.0));
		$tpv=$tpg-$tpn;

		$spn += $tpn;
		$spv += $tpv;
		$spg += $tpg;

		$_preview_values[$ln]=array(
			'positions_id'=>$rcl.'~'.$p,
			'name_projekt'=>$rcl.'~'.$tsk,
			'r_bezeichnung'=>$rcl.'~'.$rna[random_int(0,1)],
			'r_anzahl'=>$rcl.'~'.get_entry($amt,'decimal'),
			'verkaufseinheit'=>$rcl.'~'.$units[1],
			'mwst_satz'=>$rcl.'~'.get_entry($vat,'decimal').'&nbsp;%',
			'r_einzelpreis'=>$rcl.'~'.get_entry($upn,'decimal').'&nbsp;'.$GLOBALS['currency'],
			'r_gesamtpreis'=>$rcl.'~'.get_entry($tpn,'decimal').'&nbsp;'.$GLOBALS['currency'],
			'r_gesamtmwst'=>$rcl.'~'.get_entry($tpv,'decimal').'&nbsp;'.$GLOBALS['currency'],
			'r_gesamtbrutto'=>$rcl.'~'.get_entry($tpg,'decimal').'&nbsp;'.$GLOBALS['currency']
		);

		// Integrate the column "Process" into the column "Description"?
		if($sp_intprocess)
			$_preview_values[$ln]['r_bezeichnung']=$_preview_values[$ln]['name_projekt'].'<br />'.substr((string)$_preview_values[$ln]['r_bezeichnung'],1+strlen((string)$rcl));

		$ln++;

		// Add an optional comment?
		if(random_int(0,1))
		{
			$_preview_values[$ln]=array();
			if($sp_intprocess)
				$_preview_values[$ln]['positions_id']=$rcl.'~';
			else
			{
				$_preview_values[$ln]['positions_id']=substr((string)$rcl,0,-1).'" colspan="2"><!--|~';
				$_preview_values[$ln]['name_projekt']='~-->';
			}
			$val='<div style="margin:1px 5px;padding:2px 0;color:#666;white-space:normal;">'.$txt_notes.'</div>';
			$_preview_values[$ln]['r_bezeichnung']=substr((string)$rcl,0,-1).'" colspan="'.$cspan.'" style="border-top:1px dotted #'.$GLOBALS['msk_nbcol'].';border-right:1px solid #'.$GLOBALS['msk_nbcol'].';" nowrap><!--|~'.$val;
			$_preview_values[$ln][$clast]='~-->'.$val;
			$ln++;
		}

		$rv++;
	}

	// Invoice values
	$_preview_invoice=array(
		'mandanten_id'=>1, 'rechnungs_id'=>-1, 'rechnn_f'=>$txt_preview.'-'.$year.'-'.$month, 'datum'=>$GLOBALS['datetime'], 'rechnungstyp'=>0, 'rechnungsform'=>'0/cfg_prng.cfg',
		'kalktyp'=>2733, 'buendelpos'=>0, 'kto'=>'', 'ktu'=>'', 'ist_beg_dtm'=>$year.$month.'01000000', 'ist_end_dtm'=>$year.$month.'28240000', 'kunde'=>1,
		'mutterprojekt_id'=>-1, 'name_mutterprojekt'=>$_preview_subst['pname'], 'pname'=>'Test Inc.', 'vorname'=>'', 'anredetext'=>'Company', 'kundennummer'=>'CST/123456',
		'adressen_id'=>1, 'staat'=>'GB', 'plz'=>'W2HA BK', 'ort'=>'London', 'strasse'=>'Main St. 123', 'person_angelegt'=>0,
		'allg_textfeld'=>'Header text field - Test', 'allg_textfeld_fuss'=>'Footer text field - Test',
		'nettobetrag'=>$spn, 'bruttobetrag'=>$spg, 'mehrwertsteuer'=>$spv,
		'ausgegangen_am'=>'', 'bezahlt_am'=>'', 'bezahlt'=>'', 'archiv'=>'', 'aenderungs_id'=>1, 'zeitstempel'=>$GLOBALS['datetime']
	);

	// Invoice contact person
	$_preview_invoice_contact=$rna[random_int(2,3)];

	// ...Deutsch
	if($lng == 1)
	{
		$_preview_invoice['pname']='Test GmbH';
		$_preview_invoice['anredetext']='Firma';
		$_preview_invoice['kundennummer']='KNR/123456';
		$_preview_invoice['staat']='D';
		$_preview_invoice['plz']='12345';
		$_preview_invoice['ort']='Musterort';
		$_preview_invoice['strasse']='Musterstr. 123';
		$_preview_invoice['allg_textfeld']='Kopf Textfeld - Test';
		$_preview_invoice['allg_textfeld_fuss']='Fuss Textfeld - Test';
	}

	$_preview_invid=-1;
	$_preview_tr=104;
	$_preview_mask=277;
}

// ...Performance Proof (PDF)
else if($_preview_type == 2019)
{
	$weekdays=fxf_getWeekdayNames(2);
//fxDebug($weekdays,'$weekdays', 0);

	// Header literals: Day, Date, Time, Break, Project, Activity + TotalHours
	$_preview_header=array(0=>get_text(289,'5'), get_text(273,'10'), get_text(331,'10'), get_text(310,'10'), get_text(631,'10'), 6=>get_text(7,'15'), 9=>get_text(1154,'10'));

	// Mask definitions
	$_preview_madd=array(
		1=>array(1=>0,1,2,3,4,'blank',6,'blank','blank',9,'blank'),
		'lines'=>1,
		'columns'=>'1,2,3,4,5,7,10',
		'rep_headline'=>meldung(1037,false).': '.$txt_preview.' (5 '.meldung(1255,false).')'	// Performance Proof: Preview (5 Days)
	);

	// Preview values
	$_preview_values=array();
	$tot=0;
	$mnd='';
	$mxd='';
	for($i=0; $i<5; $i++)
	{
		$mxd=get_entry($year.$month.'0'.($i+1), 'date');
		if(!strlen((string)$mnd))
			$mnd=$mxd;

		$brk=random_int(2,5)*900;
		$wrk=random_int(28,36)*900;
		$stm=random_int(28,36)*900;
		$etm=$stm+$brk+$wrk;
		$tot += $wrk;

		$_preview_values[$i]['tage']='~'.$weekdays[$i+1];
		$_preview_values[$i]['datum']='~'.$mxd;
		$_preview_values[$i]['zeit_id']='~'.secToTime($stm,true).'-'.secToTime($etm,true);
		$_preview_values[$i]['pause']=$brk;
		$_preview_values[$i]['projekt']='~<font class="darkgrey">'.$txt_task.' '.$_preview_subst['pnumber'].'.1.'.($i+1).'</font>';
		$_preview_values[$i]['taetigkeit_text']='~'.$lia[random_int(0,5)];
		$_preview_values[$i]['ges_std']=$wrk;
	}

	// Grand-total
	$_preview_subst['name']=$rna[random_int(0,1)];
	$_preview_subst['timespan']=$mnd.' - '.$mxd;
	$txt='&nbsp;<b>'.meldung(1555, false).':</b>&nbsp;'.$_preview_subst['name'].'&nbsp;<i>('.$_preview_subst['timespan'].')</i>';
	$_preview_values[$i]['tage']='^class_cr3" colspan="6" nowrap style="border-top:1px solid #a4a4a4;border-right:1px solid #a4a4a4;"><!--|~'.$txt;
	$_preview_values[$i]['taetigkeit_text']='~-->'.$txt;
	$_preview_values[$i]['ges_std']='^class_cr3 bold|'.$tot.'^';
	$i++;

	$_preview_values[$i]['tage']='~|';
	$_preview_values[$i]['datum']='~|';
	$_preview_values[$i]['zeit_id']='~|';
	$_preview_values[$i]['pause']='~|';
	$_preview_values[$i]['projekt']='~|';
	$_preview_values[$i]['taetigkeit_text']='~|';
	$_preview_values[$i]['ges_std']='~|';

	$_preview_tr=114;
	$_preview_mask=294;
	$_preview_headline=$_preview_madd['rep_headline'];

	// ...Deutsch
	$_preview_subst['ppnumber']='69';
	$_preview_subst['pnumber']='69.1';
	if($lng == 1)
	{
		$_preview_subst['ppname']='Hauptprojekt';
		$_preview_subst['pname']='Teilprojekt';
	}
	// ... English
	else
	{
		$_preview_subst['ppname']='Main Project';
		$_preview_subst['pname']='Sub Project';
	}

}

// ...Travel Costs Reports (PDF)
else if($_preview_type == 2819)
{
	$weekdays=fxf_getWeekdayNames(2);

	$txt_lessmeal=meldung(3068);		// less meal costs
	$txt_sub=meldung(1436,true);		// Subtotal
	$txt_grand=meldung(1555,true);		// Grand-total
	$txt_km=meldung(2277, true);		// km

	$_preview_subst['timespan']=get_entry($year.$month.'01', 'date').' - '.get_entry($year.$month.'03', 'date');

	// Reisekosten-Variablen
	$te_types=fxf_getReferenceValues(1884);
	$te_categories=fxf_getReferenceValues(1291);
//fxDebug($te_types,'$te_types', 0); fxDebug($te_categories,'$te_categories', 0);

	// Mask definitions
	$_preview_madd=array(
		1=>array(10=>0, 20=>1, 30=>2, 40=>3, 100=>4, 110=>5, 190=>6, 200=>7, 210=>8, 220=>9, 290=>10),
		'lines'=>1,
		'columns'=>'10,20,30,40,100,110,190,200,210,220,290'
	);

	// Preview values
	$_preview_values=array();

	$txt_days=get_text(921,'10');		// Days
	$txt_daily=get_text(1889, 'text');	// Daily Allowances
	$txt_meals=get_text(1894, 'text');	// Meals initiated by the employer
	$_preview_values[0]=array(
		'tage'=>'^class_rhd" nowrap align="center" colspan="4"><!--|~&nbsp;'.$txt_days.'&nbsp;',
		'stunden'=>'~-->&nbsp;'.$txt_days.'&nbsp;',
		'pverpflegung'=>'^class_rhd" nowrap align="center" colspan="3" style="border-left:1px solid #888888;"><!--|~&nbsp;'.$txt_daily.'&nbsp;',
		'sumtag'=>'~-->&nbsp;'.$txt_daily.'&nbsp;',
		'pfrueh'=>'^class_rhd" nowrap align="center" colspan="4" style="border-left:1px solid #888888;"><!--|~&nbsp;'.$txt_meals.'&nbsp;',
		'sumist'=>'~-->&nbsp;'.$txt_meals.'&nbsp;'
	);

	$txt_date=get_text(273, '10');		// Date
	$txt_timeper=get_text(1325, '10');	// Time Per.
	$txt_hours=get_text(1877, '15');	// Hours
	$txt_board=get_text(1889, '10');	// Board
	$txt_accom=get_text(1882, '10');	// Accom.
	$txt_sum=get_text(879, '10');		// Sum
	$txt_breakf=get_text(1891, '10');	// Breakf.
	$txt_lunch=get_text(1892, '10');	// Lunch
	$txt_supper=get_text(1893, '10');	// Supper
	$_preview_values[1]=array(
		'tage'=>'^class_rhd" nowrap align="center" colspan="2"><!--|~&nbsp;'.$txt_date.'&nbsp;',
		'datum'=>'~-->&nbsp;'.$txt_date.'&nbsp;',
		'zeitraum'=>'^class_rhd" nowrap align="center|~&nbsp;'.$txt_timeper.'&nbsp;',
		'stunden'=>'^class_rhd" nowrap align="center|~&nbsp;'.$txt_hours.'&nbsp;',
		'pverpflegung'=>'^class_rhd" nowrap align="center|~&nbsp;'.$txt_board.'&nbsp;',
		'pnacht'=>'^class_rhd" nowrap align="center|~&nbsp;'.$txt_accom.'&nbsp;',
		'sumtag'=>'^class_rhd" nowrap align="center|~&nbsp;'.$txt_sum.'&nbsp;',
		'pfrueh'=>'^class_rhd" nowrap align="center|~&nbsp;'.$txt_breakf.'&nbsp;',
		'pmittag'=>'^class_rhd" nowrap align="center|~&nbsp;'.$txt_lunch.'&nbsp;',
		'pabend'=>'^class_rhd" nowrap align="center|~&nbsp;'.$txt_supper.'&nbsp;',
		'sumist'=>'^class_rhd" nowrap align="center|~&nbsp;'.$txt_sum.'&nbsp;'
	);

	$_preview_values[2]=array(
		'tage'=>'~|', 'datum'=>'~|', 'zeitraum'=>'~|', 'stunden'=>'~|', 'pverpflegung'=>'~|', 'pnacht'=>'~|', 'sumtag'=>'~|', 'pfrueh'=>'~|', 'pmittag'=>'~|', 'pabend'=>'~|', 'sumist'=>'~|'
	);

	if($lng == 1)
		$stplace='Muster-Startort&nbsp;-&gt;&nbsp;Muster-Zielort';
	else
		$stplace='Sample-Start-Place&nbsp;-&gt;&nbsp;Sample-Target-Place';
	$cstxt1='&nbsp;<u><b>'.$te_types[1981].'</b>:</u>&nbsp;<br />&nbsp;'.$te_categories[2213].'  (400&nbsp;'.$txt_km.')&nbsp;';
	$cstxt2='&nbsp;<b>'.$txt_timeper.':</b>&nbsp;<br />&nbsp;'.get_entry($year.$month.'01', 'date').', 08:00&nbsp;-&nbsp;'.get_entry($year.$month.'03', 'date').', 10:00&nbsp;';
	$cstxt3='&nbsp;<b>'.meldung(1537, false).'&nbsp;-&gt;&nbsp;'.meldung(1538, false).':</b>&nbsp;<br />&nbsp;'.$stplace.'&nbsp;';
	$_preview_values[3]=array(
		'tage'=>'^col_e8e8e8" nowrap colspan="4" align="left" style="padding:4px 2px;border-right:1px solid #c4c4c4;"><!--|~'.$cstxt1,
		'stunden'=>'~-->'.$cstxt1,
		'pverpflegung'=>'^col_e8e8e8" nowrap colspan="3" align="left" style="padding:4px 2px;border-right:1px solid #c4c4c4;"><!--|~'.$cstxt2,
		'sumtag'=>'~-->&nbsp;'.$cstxt2,
		'pfrueh'=>'^col_e8e8e8" nowrap colspan="4" align="left" style="padding:4px 2px;border-right:1px solid #c4c4c4;"><!--|~'.$cstxt3,
		'sumist'=>'~-->'.$cstxt3
	);

	if($lng == 1)
		$reason='Treffen mit Kunde';
	else
		$reason='Meeting with customer';
	$_preview_values[4]=array(
		'tage'=>'^col_e8e8e8" nowrap colspan="11" align="left" style="padding:4px 2px;border:1px solid #c4c4c4;border-left:0;"><!--|~&nbsp;'.$reason.'&nbsp;',
		'sumist'=>'~-->&nbsp;'.$reason.'&nbsp;'
	);

	$_preview_values[5]=array(
		'tage'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.$weekdays[1],
		'datum'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry($year.$month.'01', 'date').'&nbsp;',
		'zeitraum'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;8:00&nbsp;-&nbsp;24:00&nbsp;',
		'stunden'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry(16.00,'decimal').'&nbsp;h&nbsp;',
		'pverpflegung'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled checked style="opacity:1;">&nbsp;',
		'pnacht'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled style="opacity:1;">&nbsp;',
		'sumtag'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry(14.00,'decimal').'&nbsp;'.$GLOBALS['currency'].'&nbsp;',
		'pfrueh'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled style="opacity:1;">&nbsp;',
		'pmittag'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled style="opacity:1;">&nbsp;',
		'pabend'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled style="opacity:1;">&nbsp;',
		'sumist'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry(0.00,'decimal').'&nbsp;'.$GLOBALS['currency'].'&nbsp;'
	);

	$_preview_values[6]=array(
		'tage'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.$weekdays[2],
		'datum'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry($year.$month.'02', 'date').'&nbsp;',
		'zeitraum'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;0:00&nbsp;-&nbsp;24:00&nbsp;',
		'stunden'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry(24.00,'decimal').'&nbsp;h&nbsp;',
		'pverpflegung'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled checked style="opacity:1;">&nbsp;',
		'pnacht'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled style="opacity:1;">&nbsp;',
		'sumtag'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry(28.00,'decimal').'&nbsp;'.$GLOBALS['currency'].'&nbsp;',
		'pfrueh'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled style="opacity:1;">&nbsp;',
		'pmittag'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled style="opacity:1;">&nbsp;',
		'pabend'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled checked style="opacity:1;">&nbsp;',
		'sumist'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry(11.20,'decimal').'&nbsp;'.$GLOBALS['currency'].'&nbsp;',
	);

	$_preview_values[7]=array(
		'tage'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.$weekdays[3],
		'datum'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry($year.$month.'03', 'date').'&nbsp;',
		'zeitraum'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;0:00&nbsp;-&nbsp;10:00&nbsp;',
		'stunden'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry(10.00,'decimal').'&nbsp;h&nbsp;',
		'pverpflegung'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled checked style="opacity:1;">&nbsp;',
		'pnacht'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled style="opacity:1;">&nbsp;',
		'sumtag'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry(14.00,'decimal').'&nbsp;'.$GLOBALS['currency'].'&nbsp;',
		'pfrueh'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled style="opacity:1;">&nbsp;',
		'pmittag'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled style="opacity:1;">&nbsp;',
		'pabend'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;<input type="checkbox" disabled style="opacity:1;">&nbsp;',
		'sumist'=>'^col_?" nowrap valign="middle" style="padding:2px 4px;border-right:1px solid #c4c4c4;|~&nbsp;'.get_entry(0.00,'decimal').'&nbsp;'.$GLOBALS['currency'].'&nbsp;'
	);

	$_preview_values[8]=array(
		'tage'=>'^class_cr4" nowrap align="right" valign="middle" style="padding:4px;border-top:1px solid #c4c4c4;border-right:1px solid #c4c4c4;" colspan="4"><!--|~',
		'stunden'=>'~-->',
		'pverpflegung'=>'^class_cr4" nowrap align="right" valign="middle" style="padding:4px;border-top:1px solid #c4c4c4;border-right:1px solid #c4c4c4;" colspan="2"><!--|~&nbsp;<b>'.$txt_daily.':</b>&nbsp;',
		'pnacht'=>'~-->&nbsp;<b>'.$txt_daily.':</b>&nbsp;',
		'sumtag'=>'^class_cr4" nowrap align="right" valign="middle" style="padding:4px;border-top:1px solid #c4c4c4;border-right:1px solid #c4c4c4;|~&nbsp;<b>'.get_entry(56.00,'decimal').'</b>&nbsp;'.$GLOBALS['currency'].'&nbsp;',
		'pfrueh'=>'^class_cr4" nowrap align="right" valign="middle" style="padding:4px;border-top:1px solid #c4c4c4;border-right:1px solid #c4c4c4;" colspan="3"><!--|~&nbsp;<b>'.$txt_lessmeal.':</b>&nbsp;',
		'pabend'=>'~-->&nbsp;<b>'.$txt_lessmeal.':</b>&nbsp;',
		'sumist'=>'^class_cr4" nowrap align="right" valign="middle" style="padding:4px;border-top:1px solid #c4c4c4;border-right:1px solid #c4c4c4;|~&nbsp;<b>'.get_entry(11.20,'decimal').'</b>&nbsp;'.$GLOBALS['currency'].'&nbsp;'
	);

	$_preview_values[9]=array(
		'tage'=>'^class_cr0" nowrap valign="middle" style="padding:5px 4px;border-top:1px solid #c4c4c4;border-right:1px solid #c4c4c4;" colspan="10"><!--|~<b>'.$txt_sub.'</b>',
		'pabend'=>'~--><b>'.$txt_sub.'</b>',
		'sumist'=>'^class_cr0" nowrap valign="middle" style="padding:5px 4px;border-top:1px solid #c4c4c4;border-right:1px solid #c4c4c4;|~&nbsp;<b>'.get_entry(44.80,'decimal').'</b>&nbsp;'.$GLOBALS['currency'].'&nbsp;'
	);

	$_preview_values[10]=$_preview_values[2];

	$_preview_values[11]=array(
		'tage'=>'^class_cr3" nowrap valign="middle" style="padding:6px 4px;border-right:1px solid #a4a4a4;" colspan="10"><!--|~<b>'.$txt_grand.'</b>',
		'pabend'=>'~--><b>'.$txt_grand.'</b>',
		'sumist'=>'^class_cr3" nowrap valign="middle" style="padding:6px 4px;border-right:1px solid #a4a4a4;|~&nbsp;<b>'.get_entry(44.80,'decimal').'</b>&nbsp;'.$GLOBALS['currency'].'&nbsp;'
	);

	$_preview_values[12]=$_preview_values[2];

	$pdf_noheadlines=true;
	$pdf_noborders=true;
	$_preview_tr=12;
	$_preview_mask=2;
}

// ...Standard Reports (PDF)
else
{
	// Header literals: Project, Proc Type, Pln. Start Date, Pln. End Date, Planned Effort, Actual Effort, Plan-Act Remain Eff.
	$_preview_header=array(0=>get_text(46,'10'), get_text(366,'10'), 17=>get_text(130,'15'), get_text(76,'15'), 23=>get_text(297,'15'), 25=>get_text(298,'15'), 28=>get_text(1298,'20'));

	// Mask definitions
	$_preview_madd=array(
		1=>array(10=>0, 12=>1, 50=>17,18, 60=>23, 63=>25, 67=>28),
		'lines'=>1,
		'columns'=>'10,12,50,51,60,63,67',
		'rep_headline'=>'Standard: '.$txt_preview	// Standard: Preview
	);

	// Preview values
	$ln=0;
	$pca=array('^col_ececec|~', '^col_efefef|~');
	$tca=array('^col_fcfcfc|~', '^col_ffffff|~');
	$_preview_values=array();
	// Main project
	$_preview_values[$ln]=array(
		'name_projekt'=>$pca[$ln%2].'<b>'.$_preview_subst['pnumber'].'</b> - '.$_preview_subst['pname'].'</font></span>',
		'projektart'=>$pca[$ln%2].$txt_project,
		'soll_beg_dtm'=>$pca[$ln%2].get_entry($year.$month.'01', 'date'),
		'soll_end_dtm'=>$pca[$ln%2].get_entry($year.$month.'28', 'date'),
		'aufwand_soll'=>$pca[$ln%2].get_entry(400,'decimal').'&nbsp;'.$units[1],
		'aufwand_ist'=>$pca[$ln%2].get_entry(320,'decimal').'&nbsp;'.$units[1],
		'pufferaufwand'=>$pca[$ln%2].get_entry(80,'decimal').'&nbsp;'.$units[1]
	);
	$ln++;

	// Sub project 1
	$_preview_values[$ln]=array(
		'name_projekt'=>$pca[$ln%2].'<b>'.$_preview_subst['pnumber'].'.1</b> - '.$txt_subproject.' 1</font></span>',
		'projektart'=>$pca[$ln%2].$txt_subproject,
		'soll_beg_dtm'=>$pca[$ln%2].get_entry($year.$month.'01', 'date'),
		'soll_end_dtm'=>$pca[$ln%2].get_entry($year.$month.'14', 'date'),
		'aufwand_soll'=>$pca[$ln%2].get_entry(200,'decimal').'&nbsp;'.$units[1],
		'aufwand_ist'=>$pca[$ln%2].get_entry(180,'decimal').'&nbsp;'.$units[1],
		'pufferaufwand'=>$pca[$ln%2].get_entry(20,'decimal').'&nbsp;'.$units[1]
	);
	$ln++;

	// Task 1.1
	$_preview_values[$ln]=array(
		'name_projekt'=>$tca[$ln%2].'<b>'.$_preview_subst['pnumber'].'.1.1</b> - '.$txt_task.' 1.1</font></span>',
		'projektart'=>$tca[$ln%2].$txt_task,
		'soll_beg_dtm'=>$tca[$ln%2].get_entry($year.$month.'01', 'date'),
		'soll_end_dtm'=>$tca[$ln%2].get_entry($year.$month.'07', 'date'),
		'aufwand_soll'=>$tca[$ln%2].get_entry(100,'decimal').'&nbsp;'.$units[1],
		'aufwand_ist'=>$tca[$ln%2].get_entry(100,'decimal').'&nbsp;'.$units[1],
		'pufferaufwand'=>$tca[$ln%2].get_entry(0,'decimal').'&nbsp;'.$units[1]
	);
	$ln++;

	// Task 1.2
	$_preview_values[$ln]=array(
		'name_projekt'=>$tca[$ln%2].'<b>'.$_preview_subst['pnumber'].'.1.2</b> - '.$txt_task.' 1.2</font></span>',
		'projektart'=>$tca[$ln%2].$txt_task,
		'soll_beg_dtm'=>$tca[$ln%2].get_entry($year.$month.'08', 'date'),
		'soll_end_dtm'=>$tca[$ln%2].get_entry($year.$month.'14', 'date'),
		'aufwand_soll'=>$tca[$ln%2].get_entry(100,'decimal').'&nbsp;'.$units[1],
		'aufwand_ist'=>$tca[$ln%2].get_entry(80,'decimal').'&nbsp;'.$units[1],
		'pufferaufwand'=>$tca[$ln%2].get_entry(20,'decimal').'&nbsp;'.$units[1]
	);
	$ln++;

	// Sub project 2
	$_preview_values[$ln]=array(
		'name_projekt'=>$pca[$ln%2].'<b>'.$_preview_subst['pnumber'].'.2</b> - '.$txt_subproject.' 2</font></span>',
		'projektart'=>$pca[$ln%2].$txt_subproject,
		'soll_beg_dtm'=>$pca[$ln%2].get_entry($year.$month.'15', 'date'),
		'soll_end_dtm'=>$pca[$ln%2].get_entry($year.$month.'28', 'date'),
		'aufwand_soll'=>$pca[$ln%2].get_entry(200,'decimal').'&nbsp;'.$units[1],
		'aufwand_ist'=>$pca[$ln%2].get_entry(140,'decimal').'&nbsp;'.$units[1],
		'pufferaufwand'=>$pca[$ln%2].get_entry(60,'decimal').'&nbsp;'.$units[1]
	);
	$ln++;

	// Task 2.1
	$_preview_values[$ln]=array(
		'name_projekt'=>$tca[$ln%2].'<b>'.$_preview_subst['pnumber'].'.2.1</b> - '.$txt_task.' 2.1</font></span>',
		'projektart'=>$tca[$ln%2].$txt_task,
		'soll_beg_dtm'=>$tca[$ln%2].get_entry($year.$month.'15', 'date'),
		'soll_end_dtm'=>$tca[$ln%2].get_entry($year.$month.'21', 'date'),
		'aufwand_soll'=>$tca[$ln%2].get_entry(110,'decimal').'&nbsp;'.$units[1],
		'aufwand_ist'=>$tca[$ln%2].get_entry(90,'decimal').'&nbsp;'.$units[1],
		'pufferaufwand'=>$tca[$ln%2].get_entry(20,'decimal').'&nbsp;'.$units[1]
	);
	$ln++;

	// Task 2.2
	$_preview_values[$ln]=array(
		'name_projekt'=>$tca[$ln%2].'<b>'.$_preview_subst['pnumber'].'.2.2</b> - '.$txt_task.' 2.2</font></span>',
		'projektart'=>$tca[$ln%2].$txt_task,
		'soll_beg_dtm'=>$tca[$ln%2].get_entry($year.$month.'22', 'date'),
		'soll_end_dtm'=>$tca[$ln%2].get_entry($year.$month.'28', 'date'),
		'aufwand_soll'=>$tca[$ln%2].get_entry(90,'decimal').'&nbsp;'.$units[1],
		'aufwand_ist'=>$tca[$ln%2].get_entry(50,'decimal').'&nbsp;'.$units[1],
		'pufferaufwand'=>$tca[$ln%2].get_entry(40,'decimal').'&nbsp;'.$units[1]
	);
	$ln++;

	$_preview_tr=152;
	$_preview_mask=349;
	$_preview_headline=$_preview_madd['rep_headline'];
}

// Include file manager?
if(strlen((string)$incfn))
{
//fxDebug($_preview_values,'$_preview_values: $incfn='.$incfn, 0);
	$GLOBALS['__includer'][$incfn]=true;
	require('includer.inc');
}

$GLOBALS['fxpglobals']['lang']=$lng_backup;
?>