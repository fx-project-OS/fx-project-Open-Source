<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : popup_ppsp.inc                                               //
// Version     : 24.1                                                         //
// Begin       : 2020-11-02                                                   //
// Last Change : 2024-03-18                                                   //
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
 * Projects: PP/SP recalculation
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

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

$md=0;
$sf=1;
$po='';

$pr=0;
$no='';

$cp=0;
$sk='';
$hr='';
$pe='';
$pc='';
$pp='';
$sp='';

$ef=0;
$ib=0.00;
$eb=0.00;

$in=0.00;
$ix=0.00;
$en=0.00;
$ex=0.00;

if(fxIsArray($_GET))
{
	if(isset($_GET['md']))
		$md=(int)$_GET['md'];
	if(isset($_GET['sf']))
		$sf=(int)$_GET['sf'];
	if(isset($_GET['po']))
		$po=(int)$_GET['po'];

	if(isset($_GET['pr']))
		$pr=(int)$_GET['pr'];
	if(isset($_GET['no']))
		$no=$_GET['no'];

	if(isset($_GET['cp']))
		$cp=(int)$_GET['cp'];
	if(isset($_GET['sk']))
		$sk=$_GET['sk'];
	if(isset($_GET['hr']))
		$hr=$_GET['hr'];
	if(isset($_GET['pe']))
		$pe=$_GET['pe'];
	if(isset($_GET['pc']))
		$pc=$_GET['pc'];
	if(isset($_GET['pp']))
		$pp=$_GET['pp'];
	if(isset($_GET['sp']))
		$sp=$_GET['sp'];

	if(isset($_GET['ef']))
		$ef=(int)$_GET['ef'];
	if(isset($_GET['ib']))
		$ib=(float)$_GET['ib'];
	if(isset($_GET['eb']))
		$eb=(float)$_GET['eb'];

	if(isset($_GET['in']))
		$in=(float)$_GET['in'];
	if(isset($_GET['ix']))
		$ix=(float)$_GET['ix'];
	if(isset($_GET['en']))
		$en=(float)$_GET['en'];
	if(isset($_GET['ex']))
		$ex=(float)$_GET['ex'];
}
$ci=(int)$GLOBALS['fxpglobals']['client'];
$cl=(int)db_value("SELECT personen_id FROM personen WHERE personentyp=274 AND mandanten_id=".$ci);
$cu=(int)db_value("SELECT elter FROM personen WHERE personen_id=".$cp." AND mandanten_id=".$ci);
$eh=(float)$ef/3600.00;

$vex=false;
$aie=false;
if($pr > 0)
{
	$sql  = "SELECT p.aufwand_soll, p.aufwand_ist, b.budget_pers_int_gepl, b.budget_pers_int_gen, b.budget_pers_int_verbr, b.budget_pers_ext_gepl, b.budget_pers_ext_gen, b.budget_pers_ext_verbr";
	$sql .= " FROM projekte p, budget_summe b";
	$sql .= " WHERE p.projekt_id=".$pr." AND p.mandanten_id=".$ci." AND b.projekt_id=p.projekt_id AND b.mandanten_id=".$ci;
	$aie  = db_values($sql);
//fxDebug($aie,$sql);
	if(($aie['budget_pers_int_gepl'] > 0.00) || ($aie['budget_pers_ext_gepl'] > 0.00) || ($aie['budget_pers_int_verbr'] > 0.00) || ($aie['budget_pers_ext_verbr'] > 0.00))
		$vex=true;
}

$width=940;

// Hide popup?
$hp=false;
if(!$GLOBALS['fxpglobals']['setppsppopalways'] && !$vex && fxIsArray($_POST) && isset($_POST['hp']) && $_POST['hp'])
	$hp=true;

// Resources
$ar=0;
$ra=array();
if(strlen((string)$hr))
{
	while(substr((string)$hr,0,1) == '*')
		$hr=substr((string)$hr,1);
	$ha=explode(',',$hr);
	if(sizeof($ha) == 1)
		$hr=(int)$ha[0];
	foreach($ha as $c => $pers)
	{
		$ra[$c]=array('pers'=>(int)$pers, 'pe'=>0, 'pc'=>1.00, 'pp'=>0.00, 'sp'=>0.00);
		if($ra[$c]['pers'] > 0)
			$ar++;
	}
}
// ...Planned efforts per resource in seconds
if(sizeof($ra) && strlen((string)$pe))
{
	$pe=explode(',',$pe);
	foreach($pe as $c => $pv)
		$ra[$c]['pe']=(int)$pv;
}
// ...Percentage assignments per resource
if(sizeof($ra) && strlen((string)$pc))
{
	$pc=explode(',',$pc);
	foreach($pc as $c => $pv)
		$ra[$c]['pc']=(float)$pv;
}
// ...Purchase price per resource
if(sizeof($ra) && strlen((string)$pp))
{
	$pp=explode(',',$pp);
	foreach($pp as $c => $pv)
		$ra[$c]['pp']=(float)$pv;
}
// ...Sell price per resource
if(sizeof($ra) && strlen((string)$sp))
{
	$sp=explode(',',$sp);
	foreach($sp as $c => $pv)
		$ra[$c]['sp']=(float)$pv;
}
//fxDebug($ra,'$ra: $ar='.$ar);

$pe=0;
$pc=0.00;
$pp=0.00;
$sp=0.00;
if(sizeof($ra))
{
	foreach($ra as $r)
	{
		$pe += $r['pe'];
		$pc += $r['pc'];
		$pp += $r['pe']*$r['pp'];
		$sp += $r['pe']*$r['sp'];
	}
}
if($pe > 0)
{
	$pp /= $pe;
	$sp /= $pe;
}
//echo('<div style="background:#ffffff;padding:8px;border:1px solid #888888;border-radius:6px;">$hr='.$hr.'<br />$pe='.$pe.', $pc='.$pc.'<br />$pp='.$pp.', $sp='.$sp.'</div>');

$html='';

if(!$md)
{
	// Dynamic content
	$html .= '	<table width="'.($width-8).'" border="0" cellspacing="0" cellpadding="0" style="margin:4px;">'.$nl;
	$html .= '		<tr>'.$nl;
	$html .= '			<td id="ppspcontent" style="border-radius:4px;box-shadow:1px 1px 4px #bbbbbb inset;background:#f8f8f8;padding:8px;">'.$nl;
}

// Dynamic content
$budgettype=strtoupper(substr((string)$GLOBALS['fxpglobals']['settings']['budgetart'],0,1));

$average=meldung(1937, false);	// Average
$unit_h=get_text(1877, '5');	// Hr.
$unit_currency=getCurrencyUnit();

$actvala=explode('|',meldung(1936,false));	// Aktuell|Aktuell gespeicherte SOLL-Werte|Aktuell gespeicherte IST-Werte|Aktuell eingetragene SOLL-Werte

$th_ca=get_text(275, '20');		// Category
$th_nm=get_text(14, '20');		// Name
$th_pe=get_text(1584, '10');	// Percent
$th_pp=get_text(805,'text');	// Purchase Price
$th_sp=get_text(806,'text');	// Sell Price

$th_ef=get_text(274, '20');		// Time Effort
$th_ib=get_text(1812, '15');	// Int. PBudget
$th_eb=get_text(1813, '15');	// Ext. PBudget

$th_ibf=get_text(1781,'20');	// Int. HRB Frame
$th_ibu=get_text(1766,'15');	// Int. Used HRB
$th_ebf=get_text(1859,'20');	// Ext. HRB Frame
$th_ebd=get_text(1913,'15');	// Ext. Ded. HRB

$txt_ra=explode('|',meldung(3042,false));
$txt_st=$txt_ra[2];					// Standard rate

$txt_pr=get_text(66, '20');			// Process
$txt_cs=meldung(276, false);		// Customer
$txt_rs=get_text(1693, '20');		// Resource
$txt_cl=meldung(274, false);		// Client

// Your entered [<b>Planned Effort</b>] was adjusted in the marked line to prevent overspending or under budgeting (Budget = Effort * Price)! See the tooltip of the red marked new calculated value for detailed informations.
// |The <b>Planned Effort</b> was adjusted to prevent under budgeting for &quot;{FLD}&quot; of {VAL}!
// |The <b>Planned Effort</b> was adjusted to prevent overspending for &quot;{FLD1}&quot;!<br />The maxmimum amount for &quot;{FLD2}&quot; is {VAL}
$msga=explode('|',meldung(3000,false));

$skill_sid=0;
$skill_gid=0;
$skill_id=(int)$sk;
$skill_tx='';
if($skill_id)
{
	if(substr((string)$sk,-1) == 'f')
	{
		$txt_sk=$txt_ra[1];	// Skill rate
		$skill_sid=$skill_id;
		$sql="SELECT maart_id,mafaehigkeit_name FROM ma_faehigkeit WHERE mafaehigkeit_id=".$skill_sid." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$ci.")";
		$ska=db_values($sql);
		$skill_gid=(int)$ska['maart_id'];
		$sql="SELECT maart_name FROM ma_art WHERE maart_id=".$skill_gid." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$ci.")";
		$skg=db_value($sql);
		$skill_tx=strtr($skg.' &gt; '.$ska['mafaehigkeit_name'],$caption_strtr);
	}
	else
	{
		$txt_sk=$txt_ra[0];	// Skill group rate
		$skill_gid=$skill_id;
		$sql="SELECT maart_name FROM ma_art WHERE maart_id=".$skill_id." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$ci.")";
		$skill_tx=strtr(db_value($sql),$caption_strtr);
	}
}

$lines=array(
	0 => array($txt_rs.' <i class="lightgreen">('.$actvala[0].')</i>', 0, $hr, ''),							// Resource (Actual Values)
	1 => array($txt_rs.' <i class="lightblue">('.meldung(34, false).')</i>', 0, 0, ''),						// Resource (Assigned)

	2 => array($txt_pr.' <i class="darkgrey" tooltip="'.$skill_tx.'">('.$txt_sk.')</i>', 66, $pr, $sk),		// Process (Skill rate)
	3 => array($txt_pr.' <i class="lightgrey">('.$txt_st.')</i>', 66, $pr, ''),								// Process (Standard rate)

	4 => array($txt_cs.' <i class="darkgrey" tooltip="'.$skill_tx.'">('.$txt_sk.')</i>', 21, $cu, $sk),		// Customer (Skill rate)
	5 => array($txt_cs.' <i class="lightgrey">('.$txt_st.')</i>', 21, $cu, ''),								// Customer (Standard rate)

	6 => array($txt_rs.' <i class="darkgrey" tooltip="'.$skill_tx.'">('.$txt_sk.')</i>', 21, $hr, $sk),		// Resource (Skill rate)
	7 => array($txt_rs.' <i class="lightgrey">('.$txt_st.')</i>', 21, $hr, ''),								// Resource (Standard rate)

	8 => array($txt_cl.' <i class="darkgrey" tooltip="'.$skill_tx.'">('.$txt_sk.')</i>', 21, $cl, $sk),		// Client (Skill rate)
	9 => array($txt_cl.' <i class="lightgrey">('.$txt_st.')</i>', 21, $cl, '')								// Client (Standard rate)
);

// Note
$tsize='s4';
switch($sf)
{
	case 2:		// Int. Plan. HRB
		$var1 = '<font class="'.$tsize.' blue">['.$th_ef.']</font>';			// Plan. Effort
		if($budgettype == 'B')
			$var1 .= ' + <font class="'.$tsize.' blue">['.$th_eb.']</font>';	// + Ext. Plan. HRB
		$var3 = $th_ib;
		$var4 = $ib;
		$unit = $unit_currency;
	break;

	case 3:		// Ext. Plan. HRB
		$var1 = '<font class="'.$tsize.' blue">['.$th_ef.']</font>';			// Plan. Effort
		if($budgettype == 'B')
			$var1 .= ' + <font class="'.$tsize.' blue">['.$th_ib.']</font>';	// + Int. Plan. HRB
		$var3 = $th_eb;
		$var4 = $eb;
		$unit = $unit_currency;
	break;

	default:	// Plan. Effort
		$var1 = '';
		if(($budgettype == 'I') || ($budgettype == 'B'))
			$var1 = '<font class="'.$tsize.' blue">['.$th_ib.']</font>';		// Int. Plan. HRB
		if(($budgettype == 'E') || ($budgettype == 'B'))
		{
			if(strlen((string)$var1))
				$var1 .= ' + ';
			$var1 .= '<font class="'.$tsize.' blue">['.$th_eb.']</font>';		// + Ext. Plan. HRB
		}
		$var3 = $th_ef;
		$var4 = $eh;
		$unit = $unit_h;
	break;
}

$msg='';

// Colspan
$colspan=14;
if($budgettype != 'B')
	$colspan -= 4;

$table  = '				<table border="0" cellspacing="0" cellpadding="2">'.$nl;

// Headers
$headers  = '					<tr>'.$nl;
$headers .= '						<td width="100%" class="rhl" align="center" nowrap>&nbsp;<b>'.$th_ca.'</b>&nbsp;</td>'.$nl;		// Category
$headers .= '						<td>&nbsp;&nbsp;</td>'.$nl;
$headers .= '						<td class="rhl" colspan="2" align="center" nowrap>&nbsp;<b>'.$th_ef.'</b>&nbsp;</td>'.$nl;		// Effort
$headers .= '						<td>&nbsp;&nbsp;</td>'.$nl;
if(($budgettype == 'I') || ($budgettype == 'B'))
{
	$headers .= '						<td class="rhl" align="center" nowrap>&nbsp;<b>'.$th_pp.'</b>&nbsp;</td>'.$nl;				// PP
	if($budgettype == 'B')
		$headers .= '						<td>&nbsp;</td>'.$nl;
}
if(($budgettype == 'E') || ($budgettype == 'B'))
	$headers .= '						<td class="rhl" align="center" nowrap>&nbsp;<b>'.$th_sp.'</b>&nbsp;</td>'.$nl;				// SP
$headers .= '						<td>&nbsp;&nbsp;</td>'.$nl;
if(($budgettype == 'I') || ($budgettype == 'B'))
{
	$headers .= '						<td class="rhl" colspan="2" align="center" nowrap>&nbsp;<b>'.$th_ib.'</b>&nbsp;</td>'.$nl;	// Int. PB
	if($budgettype == 'B')
		$headers .= '						<td>&nbsp;</td>'.$nl;
}
if(($budgettype == 'E') || ($budgettype == 'B'))
	$headers .= '						<td class="rhl" colspan="2" align="center" nowrap>&nbsp;<b>'.$th_eb.'</b>&nbsp;</td>'.$nl;	// Ext. PB
$headers .= '					</tr>'.$nl;
$table .= $headers;


// Actual
$old_pp=0.00;
if(strlen((string)$pp))
	$old_pp=$pp;
else if($eh)
	$old_pp=$ib/$eh;

$old_sp=0.00;
if(strlen((string)$sp))
	$old_sp=$sp;
else if($eh)
	$old_sp=$eb/$eh;

$trc=0;
$values=array('ef'=>array(), 'ib'=>array(), 'eb'=>array(), 'rc'=>array());

// Aktuelle IST/SOLL-Werte
if(fxIsArray($aie))
{
	// Aktuell gespeicherte IST-Werte
	$aeh=$aie['aufwand_ist']/3600;
	$aib=$aie['budget_pers_int_verbr'];
	$aeb=$aie['budget_pers_ext_verbr'];

	$act_pp=0.00;
	$act_sp=0.00;
	if($aeh > 0)
	{
		$act_pp=$aib/$aeh;
		$act_sp=$aeb/$aeh;
	}

	$table .= '					<tr>'.$nl;
	$table .= '						<td align="left" nowrap>&nbsp;<b class="darkgrey">'.$actvala[2].'</b>&nbsp;</td>'.$nl;
	$table .= '						<td></td>'.$nl;
	$table .= '						<td></td>'.$nl;
	$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="darkgrey">'.get_entry($aeh, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_h.'</font>&nbsp;</td>'.$nl;
	$table .= '						<td>&nbsp;<font class="lightgrey">&times;</font>&nbsp;</td>'.$nl;
	if(($budgettype == 'I') || ($budgettype == 'B'))
	{
		$table .= '						<td align="right" nowrap>&nbsp;<font class="darkgrey">'.get_entry($act_pp, 'dezimal').'</font>&nbsp;<font class="lightgrey">'.$unit_currency.'/'.$unit_h.'</font>&nbsp;</td>'.$nl;
		if($budgettype == 'B')
			$table .= '						<td></td>'.$nl;
	}
	if(($budgettype == 'E') || ($budgettype == 'B'))
		$table .= '						<td align="right" nowrap>&nbsp;<font class="darkgrey">'.get_entry($act_sp, 'dezimal').'</font>&nbsp;<font class="lightgrey">'.$unit_currency.'/'.$unit_h.'</font>&nbsp;</td>'.$nl;
	$table .= '						<td>&nbsp;<font class="lightgrey">=</font>&nbsp;</td>'.$nl;
	if(($budgettype == 'I') || ($budgettype == 'B'))
	{
		$table .= '						<td></td>'.$nl;
		$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="darkgrey">'.get_entry($aib, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_currency.'</font>&nbsp;</td>'.$nl;
		if($budgettype == 'B')
			$table .= '						<td></td>'.$nl;
	}
	if(($budgettype == 'E') || ($budgettype == 'B'))
	{
		$table .= '						<td></td>'.$nl;
		$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="darkgrey">'.get_entry($aeb, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_currency.'</font>&nbsp;</td>'.$nl;
	}
	$table .= '					</tr>'.$nl;

	// ...Aktuell gspeicherte SOLL-Werte
	$peh=$aie['aufwand_soll']/3600;
	$pib=$aie['budget_pers_int_gepl'];
	$peb=$aie['budget_pers_ext_gepl'];

	$plan_pp=0.00;
	$plan_sp=0.00;
	if($peh > 0)
	{
		$plan_pp=$pib/$peh;
		$plan_sp=$peb/$peh;
	}

	$table .= '					<tr>'.$nl;
	$table .= '						<td align="left" nowrap>&nbsp;<b class="darkgrey">'.$actvala[1].'</b>&nbsp;</td>'.$nl;
	$table .= '						<td></td>'.$nl;
	$table .= '						<td></td>'.$nl;
	$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="darkgrey">'.get_entry($peh, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_h.'</font>&nbsp;</td>'.$nl;
	$table .= '						<td>&nbsp;<font class="lightgrey">&times;</font>&nbsp;</td>'.$nl;
	if(($budgettype == 'I') || ($budgettype == 'B'))
	{
		$table .= '						<td align="right" nowrap>&nbsp;<font class="darkgrey">'.get_entry($plan_pp, 'dezimal').'</font>&nbsp;<font class="lightgrey">'.$unit_currency.'/'.$unit_h.'</font>&nbsp;</td>'.$nl;
		if($budgettype == 'B')
			$table .= '						<td></td>'.$nl;
	}
	if(($budgettype == 'E') || ($budgettype == 'B'))
		$table .= '						<td align="right" nowrap>&nbsp;<font class="darkgrey">'.get_entry($plan_sp, 'dezimal').'</font>&nbsp;<font class="lightgrey">'.$unit_currency.'/'.$unit_h.'</font>&nbsp;</td>'.$nl;
	$table .= '						<td>&nbsp;<font class="lightgrey">=</font>&nbsp;</td>'.$nl;
	if(($budgettype == 'I') || ($budgettype == 'B'))
	{
		$table .= '						<td></td>'.$nl;
		$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="darkgrey">'.get_entry($pib, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_currency.'</font>&nbsp;</td>'.$nl;
		if($budgettype == 'B')
			$table .= '						<td></td>'.$nl;
	}
	if(($budgettype == 'E') || ($budgettype == 'B'))
	{
		$table .= '						<td></td>'.$nl;
		$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="darkgrey">'.get_entry($peb, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_currency.'</font>&nbsp;</td>'.$nl;
	}
	$table .= '					</tr>'.$nl;

	$table .= '					<tr>'.$nl;
	$table .= '						<td colspan="'.$colspan.'" style="border-top:1px solid #aaaaaa;"></td>'.$nl;
	$table .= '					</tr>'.$nl;
}

// Aktuell eingetragene SOLL-Werte
$cal_pp=0.00;
$cal_sp=0.00;
if($eh > 0)
{
	$cal_pp=$ib/$eh;
	$cal_sp=$eb/$eh;
}
$table .= '					<tr style="cursor:default;" '.fxf_jsFunction('highlightLine', '*onmouseover', '$(this)', 1).' '.fxf_jsFunction('highlightLine', '*onmouseout', '$(this)', 0).'>'.$nl;
$table .= '						<td id="tdppsp_'.$trc.'" align="left" nowrap style="cursor:pointer;">&nbsp;<b id="tdppsp_'.$trc.'" class="green">'.$actvala[3].'</b>&nbsp;</td>'.$nl;
$table .= '						<td></td>'.$nl;
$table .= '						<td><input type="radio" name="ppspnef" value="'.$ef.'" checkedef0></td>'.$nl;
$values['ef'][0]=$ef;
$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="green">'.get_entry($eh, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_h.'</font>&nbsp;</td>'.$nl;
$table .= '						<td>&nbsp;<font class="lightgrey">&times;</font>&nbsp;</td>'.$nl;
if(($budgettype == 'I') || ($budgettype == 'B'))
{
	$table .= '						<td align="right" nowrap>&nbsp;<font class="green">'.get_entry($cal_pp, 'dezimal').'</font>&nbsp;<font class="lightgrey">'.$unit_currency.'/'.$unit_h.'</font>&nbsp;</td>'.$nl;
	if($budgettype == 'B')
		$table .= '						<td></td>'.$nl;
}
if(($budgettype == 'E') || ($budgettype == 'B'))
	$table .= '						<td align="right" nowrap>&nbsp;<font class="green">'.get_entry($cal_sp, 'dezimal').'</font>&nbsp;<font class="lightgrey">'.$unit_currency.'/'.$unit_h.'</font>&nbsp;</td>'.$nl;
$table .= '						<td>&nbsp;<font class="lightgrey">=</font>&nbsp;</td>'.$nl;
if(($budgettype == 'I') || ($budgettype == 'B'))
{
	$table .= '						<td><input type="radio" name="ppspnib" value="'.$ib.'" checkedib0></td>'.$nl;
	$values['ib'][0]=$ib;
	$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="green">'.get_entry($ib, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_currency.'</font>&nbsp;</td>'.$nl;
	if($budgettype == 'B')
		$table .= '						<td></td>'.$nl;
}
if(($budgettype == 'E') || ($budgettype == 'B'))
{
	$table .= '						<td><input type="radio" name="ppspneb" value="'.$eb.'" checkedeb0></td>'.$nl;
	$values['eb'][0]=$eb;
	$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="green">'.get_entry($eb, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_currency.'</font>&nbsp;</td>'.$nl;
}
$table .= '					</tr>'.$nl;

// Differenz
if(fxIsArray($aie))
{
	$deh=$eh-$peh;
	$dib=$ib-$pib;
	$deb=$eb-$peb;

	if($deh || $dib || $deb)
	{
		$dcs=5;
		if($budgettype != 'B')
			$dcs -= 2;

		$table .= '					<tr>'.$nl;
		$table .= '						<td colspan="3"></td>'.$nl;
		if($deh)
		{
			$dvz='+';
			if($deh < 0)
			{
				$dvz='-';
				$deh=-$deh;
			}
			$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="red">'.$dvz.get_entry($deh, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_h.'</font>&nbsp;</td>'.$nl;
		}
		else
			$table .= '						<td></td>'.$nl;
		$table .= '						<td colspan="'.$dcs.'"></td>'.$nl;
		if(($budgettype == 'I') || ($budgettype == 'B'))
		{
			$table .= '						<td></td>'.$nl;
			if($dib)
			{
				$dvz='+';
				if($dib < 0)
				{
					$dvz='-';
					$dib=-$dib;
				}
				$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="red">'.$dvz.get_entry($dib, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_currency.'</font>&nbsp;</td>'.$nl;
			}
			else
				$table .= '						<td></td>'.$nl;
			if($budgettype == 'B')
				$table .= '						<td></td>'.$nl;
		}
		if(($budgettype == 'E') || ($budgettype == 'B'))
		{
			$table .= '						<td></td>'.$nl;
			if($deb)
			{
				$dvz='+';
				if($deb < 0)
				{
					$dvz='-';
					$deb=-$deb;
				}
				$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b class="red">'.$dvz.get_entry($deb, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_currency.'</font>&nbsp;</td>'.$nl;
			}
			else
				$table .= '						<td></td>'.$nl;
		}
		$table .= '					</tr>'.$nl;
	}
}

// 1935: $var1 anpassen von Aufgabe $var2 anhand von neu eingegebenem $var3 von $var4:|$var1 anpassen von Aufgabe $var2 anhand $var3 von $var4:
$texta=explode('|', meldung(1935, false, $var1, '<b class="'.$tsize.'">'.$no.'</b>', '&quot;'.$var3.'&quot;', '<font class="'.$tsize.' green">'.get_entry($var4, 'dezimal').'</font> '.$unit));
if(fxIsArray($_POST) && isset($_POST['hp']) && $_POST['hp'])
	$text=$texta[0];
else
	$text=$texta[1];
$table .= '					<tr>'.$nl;
$table .= '						<td colspan="'.$colspan.'">&nbsp;</td>'.$nl;
$table .= '					</tr>'.$nl;
$table .= '					<tr>'.$nl;
$table .= '						<td colspan="'.$colspan.'" style="border:1px solid #888888;border-top-left-radius:6px;border-top-right-radius:6px;padding:6px;background:#eeeeee;"><br /><font class="'.$tsize.'">'.$text.'</font><br /><br /></td>'.$nl;
$table .= '					</tr>'.$nl;
$table .= '					<tr>'.$nl;
$table .= '						<td colspan="'.$colspan.'"></td>'.$nl;
$table .= '					</tr>'.$nl;

$table .= $headers;

$rsset=false;
$dlset=false;

// Kalkulationswerte
$rekvk=array();
foreach($lines as $calc_mode => $calcArr)
{
	$skip=false;
	if(!$skip && !$ar && (($calc_mode == 6) || ($calc_mode == 7)))	// No resource
		$skip=true;
	if(!$skip && !$skill_id && (($calc_mode == 2) || ($calc_mode == 4) || ($calc_mode == 6) || ($calc_mode == 8)))	// No skill
		$skip=true;
	if($skip)
		continue;

	$calc_tx=trim((string)$calcArr[0]);
	$calc_md=(int)$calcArr[1];
	$calc_id=$calcArr[2];
	$calc_sk=trim((string)$calcArr[3]);

	$delta=false;
	$pers_arr=false;

	// Ressourcen handeln
	$ekvk_arr=array('ek'=>"", 'vk'=>"");
	if(!$calc_mode || ($calc_mode == 6) || ($calc_mode == 7))	// Ressource (Aktuell, Fähigkeitensatz oder Standardsatz)
	{
		if(!$ar)	// Keine Ressource
			$ekvk_arr=array('ek'=>$pp, 'vk'=>$sp);
		else
		{
			$gp=true;
			$psa=array();
			if($calc_mode == 6)
			{
				while(true)
				{
					$sql="SELECT id, ek, vk FROM ekvk WHERE id IN (".$hr.") AND feldtyp=21 AND mafaehigkeit_id=".$skill_sid." AND maart_id=".$skill_gid." AND mandanten_id=".$ci;
					$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
						{
							if(strlen((string)$t['ek']) || strlen((string)$t['vk']))
							{
								$tid=(int)$t['id'];
								if(!isset($psa[$tid]))
									$psa[$tid]=array('ek'=>$t['ek'], 'vk'=>$t['vk']);
								else
								{
									if(!strlen((string)$psa[$tid]['ek']) && strlen((string)$t['ek']))
										$psa[$tid]['ek']=$t['ek'];
									if(!strlen((string)$psa[$tid]['vk']) && strlen((string)$t['vk']))
										$psa[$tid]['vk']=$t['vk'];
								}
							}
						}
					}
					if($skill_sid)
						$skill_sid=0;
					else
						break;
				}
//fxDebug($psa,'$psa');
				if(!sizeof($psa))	// Keine Fähigkeitseinträge gefunden
					$gp=false;
			}
			if($gp)
			{
				$pna=array();
				$sql  = "SELECT p.personen_id, p.pname, p.vorname, a.ek, a.vk";
				$sql .= " FROM personen p";
				$sql .= " RIGHT JOIN artikel a ON (a.personen_id=p.personen_id AND (a.sachmittel_id IS NULL OR a.sachmittel_id=0) AND (a.archiv IS NULL OR a. archiv=0) AND a.mandanten_id=".$ci.")";
				$sql .= " WHERE p.personen_id IN (".$hr.") AND p.mandanten_id=".$ci;
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
						$pna[(int)$t['personen_id']]=$t;
				}
				if($ar > 1)
					$delta=true;
				$ekvk_arr=array('ek'=>0.00, 'vk'=>0.00);
				$pers_arr=array();
				foreach($ra as $rc => $rd)
				{
					$tid=(int)$rd['pers'];
					$pname='?';
					$vorname='?';
					if(isset($pna[$tid]))
					{
						$pname=$pna[$tid]['pname'];
						$vorname=$pna[$tid]['vorname'];
					}
					if($calc_mode)	// Ressource (Fähigkeitensatz oder Standardsatz)
					{
						$rpp=0.00;
						$rsp=0.00;
						if(isset($pna[$tid]))
						{
							$rpp=$pna[$tid]['ek'];
							$rsp=$pna[$tid]['vk'];
						}
						if(isset($psa[$tid]))
						{
							if(strlen((string)$psa[$tid]['ek']))
								$rpp=$psa[$tid]['ek'];
							if(strlen((string)$psa[$tid]['vk']))
								$rpp=$psa[$tid]['vk'];
						}
					}
					else
					{
						$rpp=$rd['pp'];
						$rsp=$rd['sp'];
					}
					$pers_arr[$rc]=array('personen_id'=>$tid, 'pname'=>$pname, 'vorname'=>$vorname, 'ek_projekt'=>$rpp, 'vk_projekt'=>$rsp, 'prozent'=>$rd['pc']);
					$ekvk_arr['ek'] += $rpp*$rd['pc'];
					$ekvk_arr['vk'] += $rsp*$rd['pc'];
				}
			}
		}
	}
	else if($calc_mode == 1)	// Ressource (Zugeordnet)
	{
		$sql  = "SELECT a.personen_id, p.pname, p.vorname, az.ek_projekt, az.vk_projekt, m.prozent";
		$sql .= " FROM personen p, maplanung m, artikelzuordnung az, artikel a";
		$sql .= " WHERE p.personen_id=a.personen_id AND (a.sachmittel_id IS NULL OR a.sachmittel_id=0) AND az.artikel_id=a.artikel_id";
		$sql .= " AND az.projekt_id=".$pr." AND (az.historie IS NULL OR az.historie=0)";
		$sql .= " AND m.projekt_id=".$pr." AND m.personen_id=a.personen_id";
		$sql .= " AND az.mandanten_id=".$ci." AND a.mandanten_id=".$ci." AND p.mandanten_id=".$ci." AND m.mandanten_id=".$ci;
		$sql .= " ORDER BY p.pname, p.vorname";
		$pers_arr=db_values($sql,'*');
		if(fxIsArray($pers_arr))
		{
			if(sizeof($pers_arr) > 1)
			{
				$delta=true;
				$ekvk_arr=array('ek'=>0.00, 'vk'=>0.00);
				foreach($pers_arr as $pcnt => $pval)
				{
					$prz=get_double(min(1.00, max(0.00, $pval['prozent'])));
					$pers_arr[$pcnt]['prozent']=$prz;

					$ekvk_arr['ek'] += $pval['ek_projekt'] * $prz;
					$ekvk_arr['vk'] += $pval['vk_projekt'] * $prz;
				}
			}
			else
			{
				$ekvk_arr=array('ek'=>$pers_arr[0]['ek_projekt'], 'vk'=>$pers_arr[0]['vk_projekt']);
				$calc_id=(int)$pers_arr[0]['personen_id'];
				$pers_arr=false;
			}
		}
	}
	else
	{
		$ekvk_arr=calc_ppsp($calc_md, $calc_id, $pr, $calc_sk);
//fxDebug($ekvk_arr, $calc_mode.': calc_ppsp('.$calc_md.', '.$calc_id.', '.$pr.', '.$calc_sk.')');
	}

	$new_pp=$ekvk_arr['ek'];
	$new_sp=$ekvk_arr['vk'];

	if(!$calc_mode && $ar)	// Ressource (Actual)
	{
		if(strlen((string)$pp))
			$new_pp=$pp;
		if(strlen((string)$sp))
			$new_sp=$sp;
	}

	$cont=false;
	if(!strlen((string)$new_pp) && !strlen((string)$new_sp))
		$cont=true;
//if($cont) $calc_tx .= ' (*SKIP*)';
	if($cont)
		continue;

	// Neuer Kalkulationseintrag
	if(strlen((string)$new_pp))
		$calc_pp=round(get_double($new_pp),2);
	if(strlen((string)$new_sp))
		$calc_sp=round(get_double($new_sp),2);
	if(!strlen((string)$new_pp))
		$calc_pp=$old_pp;
	if(!strlen((string)$new_sp))
		$calc_sp=$old_sp;
	if(!$new_pp && !$calc_pp && !$new_sp && !$calc_sp)
		continue;
//echo('<div style="background:#ffffff;padding:8px;border:1px solid #888888;border-radius:6px;"><b>'.$calc_mode.'</b>['.$sf.'] ('.$lines[$calc_mode][0].')<br /><font class="blue">$new_pp='.$new_pp.' - $calc_pp='.$calc_pp.', $new_sp='.$new_sp.' - $calc_sp='.$calc_sp.'</font></div>');

	switch($sf)
	{
		case 2:		// Int. gepl. Personenbudget
			$new_eh=0.00;
			if($calc_pp)
				$new_eh=floor($ib/$calc_pp);

			$new_ib=$ib;

			$new_ef=(int)(floor($new_eh)*3600);
			$new_eb=round($calc_sp*$new_eh,2);
		break;

		case 3:		// Ext. gepl. Personenbudget
			$new_eh=0.00;
			if($calc_sp)
				$new_eh=floor($eb/$calc_sp);

			$new_eb=$eb;

			$new_ef=(int)(floor($new_eh)*3600);
			$new_ib=round($calc_pp*$new_eh,2);
		break;

		default:	// Aufwand Soll
			$new_eh=(double)$ef/3600.00;
			$new_ef=$ef;

			$new_ib=round($calc_pp*$new_eh,2);
			$new_eb=round($calc_sp*$new_eh,2);
		break;
	}
//echo('<div style="background:#ffffff;padding:8px;border:1px solid #888888;border-radius:6px;"><b>'.$calc_mode.'</b> ('.$lines[$calc_mode][0].')<br /><font class="blue">$in='.$in.', $ix='.$ix.' - $new_ib='.$new_ib.'<br />$en='.$en.', $ex='.$ex.' - $new_eb='.$new_eb.'</font></div>');

	$recalc=0;
	$eht='';
	if(($in > 0.00) && ($new_ib < $in))
	{
		$new_eh=floor($in/$calc_pp);
		$recalc |= 1;
		$eht=$msga[1];
		// The <b>Planned Effort</b> was adjusted to prevent under budgeting for &quot;{FLD}&quot; of {VAL}!
		$eht=strtr($msga[1],array('{FLD}'=>$th_ibu, '{VAL}'=>get_entry($in,'dezimal').' '.$unit_currency));
	}
	else if(($ix > 0.00) && ($new_ib > $ix))
	{
		$new_eh=floor($ix/$calc_pp);
		$recalc |= 2;
		$eht=$msga[2];
		// The <b>Planned Effort</b> was adjusted to prevent overspending for &quot;{FLD1}&quot;!<br />The maxmimum amount for &quot;{FLD2}&quot; is {VAL}
		$eht=strtr($msga[2],array('{FLD1}'=>$th_ibf, '{FLD2}'=>$th_ib, '{VAL}'=>get_entry($ix,'dezimal').' '.$unit_currency));
	}
	if(($en > 0.00) && ($new_eb < $en))
	{
		$new_eh=floor($en/$calc_sp);
		$recalc |= 4;
		// The <b>Planned Effort</b> was adjusted to prevent under budgeting for &quot;{FLD}&quot; of {VAL}!
		$eht=strtr($msga[1],array('{FLD}'=>$th_ebd, '{VAL}'=>get_entry($en,'dezimal').' '.$unit_currency));
	}
	else if(($ex > 0.00) && ($new_eb > $ex))
	{
		$new_eh=floor($ex/$calc_sp);
		$recalc |= 8;
		// The <b>Planned Effort</b> was adjusted to prevent overspending for &quot;{FLD1}&quot;!<br />The maxmimum amount for &quot;{FLD2}&quot; is {VAL}
		$eht=strtr($msga[2],array('{FLD1}'=>$th_ebf, '{FLD2}'=>$th_eb, '{VAL}'=>get_entry($ex,'dezimal').' '.$unit_currency));
	}
	if($recalc > 0)
	{
		$values['rc'][$calc_mode]=true;
		$new_ef=(int)(floor($new_eh)*3600);
		$new_ib=round($calc_pp*$new_eh,2);
		if(($in > 0.00) && ($new_ib < $in))
			$new_ib=$in;
		else if(($ix > 0.00) && ($new_ib > $ix))
			$new_ib=$ix;
		$new_eb=round($calc_sp*$new_eh,2);
		if(($en > 0.00) && ($new_eb < $en))
			$new_eb=$en;
		else if(($ex > 0.00) && ($new_eb > $ex))
			$new_eb=$ex;

		if(!strlen((string)$msg))
			$msg='<br /><br /><div class="darkgrey" style="border-top:1px dotted #cccccc;padding-top:4px;"><table border="0" cellpadding="0" cellspacing="0"><tr><td valign="top"><b>*</b>)&nbsp;&nbsp;</td><td>'.$msga[0].'</td></tr></table></div><br />';
	}

	// Tooltip
	$tooltip='';
	switch($calc_mode)
	{
		case 2:	// Project
		case 3:
			$sql="SELECT vorgangsnummer, name_projekt, projektart FROM projekte WHERE projekt_id=".$calc_id." AND mandanten_id=".$ci;
			$dat=db_values($sql);
			if(fxIsArray($dat))
				$tooltip='<b>'.no_null($dat['vorgangsnummer']).'</b>: '.strtr($dat['name_projekt'], $caption_strtr);
		break;

		case 4:	// Customer
		case 5:
			if($calc_id)
				$tooltip=person_pic($calc_id, 20, 0, $ci, false);
		break;

		case 8:	// Client
		case 9:
			$id=(int)db_value("SELECT personen_id FROM personen WHERE personentyp=274 AND mandanten_id=".$ci);
			if($id)
				$tooltip=person_pic($id, 20, 0, $ci, false);
		break;

		default:	// Person
			if(fxIsArray($pers_arr) && (sizeof($pers_arr) > 1))
			{
//fxDebug($pers_arr,'$pers_arr');
				$tooltip  = '<table border=0 cellpadding=1 cellspacing=0>';
				$tooltip .= '<tr>';
				$tooltip .= '<td class=rhds colspan=2 align=center nowrap>&nbsp;'.$th_nm.'&nbsp;</td>';	// Name
				$tooltip .= '<td class=rhds align=center nowrap>&nbsp;'.$th_pe.'&nbsp;</td>';			// Percent
				if(($budgettype == 'I') || ($budgettype == 'B'))
				{
					$cls='';
					if($budgettype == 'B')
						$cls=' class=rhds';
					$tooltip .= '<td'.$cls.' align=center nowrap>&nbsp;'.$th_pp.'&nbsp;</td>';			// Purchase Price
				}
				if(($budgettype == 'E') || ($budgettype == 'B'))
					$tooltip .= '<td class=rhd align=center nowrap>&nbsp;'.$th_sp.'&nbsp;</td>';		// Sell Price
				$tooltip .= '</tr>';
				foreach($pers_arr as $pcnt => $pdat)
				{
					$pers_name = trim((string)$pdat['pname']);
					$pers_cname = trim((string)$pdat['vorname']);
					if(strlen((string)$pers_cname))
						$pers_name .= ', '.$pers_cname;

					$tooltip .= '<tr>';
					$tooltip .= '<td>&nbsp;'.person_pic($pdat['personen_id'], 20, 0, $ci, false).'</td>';
					$tooltip .= '<td class=rbr nowrap>&nbsp;<b>'.strtr($pers_name, $caption_strtr).'</b>&nbsp;</td>';
					$tooltip .= '<td class=rbr align=right nowrap>&nbsp;'.get_entry($pdat['prozent']*100, 'dezimal').'&nbsp;<font class=lightgrey>%</font>&nbsp;</td>';
					if(($budgettype == 'I') || ($budgettype == 'B'))
					{
						$cls='';
						if($budgettype == 'B')
							$cls=' class=rbr';
						$tooltip .= '<td'.$cls.' align=right nowrap>&nbsp;'.get_entry($pdat['ek_projekt'], 'dezimal').'&nbsp;<font class=lightgrey>'.$unit_currency.'/'.$unit_h.'</font>&nbsp;</td>';
					}
					if(($budgettype == 'E') || ($budgettype == 'B'))
						$tooltip .= '<td align=right nowrap>&nbsp;'.get_entry($pdat['vk_projekt'], 'dezimal').'&nbsp;<font class=lightgrey>'.$unit_currency.'/'.$unit_h.'</font></td>';
					$tooltip .= '</tr>';
				}
				$tooltip .= '<tr>';
				$tooltip .= '<td class=\'rbt rbr\' colspan=3 align=right>&Delta;&nbsp;</td>';
				if(($budgettype == 'I') || ($budgettype == 'B'))
				{
					$cls='rbt';
					if($budgettype == 'B')
						$cls .= ' rbr';
					$tooltip .= '<td class=\''.$cls.'\' align=right>&nbsp;<b>'.get_entry($ekvk_arr['ek'], 'dezimal').'</b>&nbsp;<font class=lightgrey>'.$unit_currency.'/'.$unit_h.'</font>&nbsp;</td>';
				}
				if(($budgettype == 'E') || ($budgettype == 'B'))
					$tooltip .= '<td class=rbt align=right>&nbsp;<b>'.get_entry($ekvk_arr['vk'], 'dezimal').'</b>&nbsp;<font class=lightgrey>'.$unit_currency.'/'.$unit_h.'</font></td>';
				$tooltip .= '</tr>';
				$tooltip .= '</table>';
			}
			else
			{
				if(fxIsArray($pers_arr))
					$id=(int)$pers_arr[0]['personen_id'];
				else
					$id=$calc_id;

				if($id)
					$tooltip=person_pic($id, 20, 0, $ci, false);
			}
		break;
	}
	if(strlen((string)$tooltip))
		$tooltip=' tooltip="'.str_replace('"','',$tooltip).'" style="cursor:pointer;"';

	if($rsset && !$dlset && ($calc_mode > 1))
	{
		$table .= '					<tr>'.$nl;
		$table .= '						<td colspan="'.$colspan.'" style="border-top:1px dotted #888888;"></td>'.$nl;
		$table .= '					</tr>'.$nl;
		$dlset=true;
	}
	if(!$rsset && ($calc_mode <= 1))
		$rsset=true;

	$table .= '					<tr style="cursor:default;" '.fxf_jsFunction('highlightLine', '*onmouseover', '$(this)', 1).' '.fxf_jsFunction('highlightLine', '*onmouseout', '$(this)', 0).'">'.$nl;

	$trc++;
	$table .= '						<td id="tdppsp_'.$trc.'" align="left" nowrap'.$tooltip.'>';
	if(strlen((string)$eht))
		$table .= '&nbsp;<b class="red">*</b>';
	$table .= '&nbsp;'.$calc_tx;
	if($delta)
		$table .= '&nbsp;&Delta;';
	if(strlen((string)$tooltip))
		$table .= '&nbsp;<img id="tdppsp_'.$trc.'" id="ttdummy" src="'.$GLOBALS['gfxpath'].'tts.png" border="0" align="top"'.$tooltip.'>';
	$table .= '&nbsp;</td>'.$nl;
	$table .= '						<td></td>'.$nl;

	$table .= '						<td><input type="radio" name="ppspnef" value="'.$new_ef.'" checkedef'.($calc_mode+1).'></td>'.$nl;
	$values['ef'][$calc_mode+1]=$new_ef;
	$cls='black';
	$stl='';
	if(strlen((string)$eht))
	{
		$cls='red';
		$stl=' tooltip="'.$eht.'" style="cursor:help;"';
	}
	$table .= '						<td align="right" nowrap'.$stl.'>&nbsp;&nbsp;<b class="'.$cls.'">'.get_entry($new_eh, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_h.'</font>&nbsp;</td>'.$nl;

	$table .= '						<td>&nbsp;<font class="lightgrey">&times;</font>&nbsp;</td>'.$nl;

	if(($budgettype == 'I') || ($budgettype == 'B'))
	{
		$cls = 'grey';
		if(strlen((string)$new_pp))
			$cls = 'black';
		$table .= '						<td align="right" nowrap>&nbsp;<font class="'.$cls.'">'.get_entry($calc_pp, 'dezimal').'</font>&nbsp;<font class="lightgrey">'.$unit_currency.'/'.$unit_h.'</font>&nbsp;</td>'.$nl;
		if($budgettype == 'B')
			$table .= '						<td></td>'.$nl;
	}

	if(($budgettype == 'E') || ($budgettype == 'B'))
	{
		$cls = 'grey';
		if(strlen((string)$new_sp))
			$cls = 'black';
		$table .= '						<td align="right" nowrap>&nbsp;<font class="'.$cls.'">'.get_entry($calc_sp, 'dezimal').'</font>&nbsp;<font class="lightgrey">'.$unit_currency.'/'.$unit_h.'</font>&nbsp;</td>'.$nl;
	}
	$table .= '						<td>&nbsp;<font class="lightgrey">=</font>&nbsp;</td>'.$nl;

	if(($budgettype == 'I') || ($budgettype == 'B'))
	{
		$table .= '						<td><input type="radio" name="ppspnib" value="'.$new_ib.'" checkedib'.($calc_mode+1).'></td>'.$nl;
		$values['ib'][$calc_mode+1]=$new_ib;
		$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b>'.get_entry($new_ib, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_currency.'</font>&nbsp;</td>'.$nl;
		if($budgettype == 'B')
			$table .= '						<td></td>'.$nl;
	}

	if(($budgettype == 'E') || ($budgettype == 'B'))
	{
		$table .= '						<td><input type="radio" name="ppspneb" value="'.$new_eb.'" checkedeb'.($calc_mode+1).'></td>'.$nl;
		$values['eb'][$calc_mode+1]=$new_eb;
		$table .= '						<td align="right" nowrap>&nbsp;&nbsp;<b>'.get_entry($new_eb, 'dezimal').'</b>&nbsp;<font class="lightgrey">'.$unit_currency.'</font>&nbsp;</td>'.$nl;
	}

	$table .= '					</tr>'.$nl;
}

$table .= '				</table>'.$nl;
//fxDebug($values,'$values');

$adjust_ext_budget=false;
if(strtoupper(substr((string)$GLOBALS['fxpglobals']['settings']['ekvkpop_ebud'],0,1)) == 'J')
	$adjust_ext_budget=true;

$found_ef=0;
$found_ib=0;
$found_eb=0;
if($sf == 2)		// Int. PB verändert		-> Aufwand Soll anpassen (+ Ext. PB, falls kein gespeicherter Soll-Wert vorhanden ist oder entspr. Einstellung)
{
	$fv1='ef';
	$fv2='ef';
	if(!$peh || $adjust_ext_budget)
		$fv2='eb';
}
else if($sf == 3)	// Ext. PB verändert		-> Int. PB + Aufwand Soll anpassen
{
	$fv1='ib';
	$fv2='ef';
}
else				// Aufwand Soll verändert	-> Int. PB anpassen (+ Ext. PB, falls kein gespeicherter Soll-Wert vorhanden ist oder entspr. Einstellung)
{
	$fv1='ib';
	$fv2='ib';
	if(!$peh || $adjust_ext_budget)
		$fv2='eb';
}
foreach($values as $vm => $va)
{
	foreach($va as $vp => $vv)
	{
		if(($vp > 0) && (($vm == $fv1) || ($vm == $fv2)))
		{
			if(!${'found_'.$vm})
				${'found_'.$vm}=$vp;
		}
	}
}
$ra=array();
foreach($values as $vm => $va)
{
	foreach($va as $vp => $vv)
	{
		if(${'found_'.$vm} == $vp)
			$ra['checked'.$vm.$vp.'>']='checked>';
		else
			$ra[' checked'.$vm.$vp.'>']='>';
	}
}
//fxDebug($ra,'$ra');
$table=strtr($table,$ra);

if($hp && (!sizeof($values['rc']) || (!isset($values['rc'][$found_ef]) && !isset($values['rc'][$found_ib]) && !isset($values['rc'][$found_eb]))))
{
	$html  = '<input type="radio" name="ppspnef" value="'.$values['ef'][$found_ef].'" checked>';
	$html .= '<input type="radio" name="ppspnib" value="'.$values['ib'][$found_ib].'" checked>';
	$html .= '<input type="radio" name="ppspneb" value="'.$values['eb'][$found_eb].'" checked>';
}
else
{
	$html .= $table;
	if(strlen((string)$msg))
		$html .= $msg.$nl;

	if(!$md)
	{
		$html .= '			</td>'.$nl;
		$html .= '		</tr>'.$nl;
		$html .= '	</table>'.$nl;

		// Button
		$html .= '	<table width="'.($width-8).'" border="0" cellspacing="0" cellpadding="0" style="margin:0 4px;">'.$nl;
		$html .= '		<tr>'.$nl;
		$html .= '			<td align="center" height="40">'.fieldSubmit('ppspok','OK','sub',100).'</td>'.$nl;
		$html .= '		</tr>'.$nl;
		$html .= '	</table>'.$nl;

		$mtitle=meldung(1934,true);	// Recalculate planning field
		$html=popupMask($mtitle, '', $html, '', $width, -1);
	}
}

if(strlen((string)$po))
	$html .= '<input type="hidden" id="ppsppc" name="ppsppc" value="'.$po.'">';

//	echo('<pre style="background:#ffffff;">'.fxHtmlEncode($html).'</pre>');
echo($html);
?>