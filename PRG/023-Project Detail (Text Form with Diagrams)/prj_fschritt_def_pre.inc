<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_fschritt_def_pre.inc                                     //
// Version     : 21.1                                                         //
// Begin       : 2020-10-29                                                   //
// Last Change : 2020-10-29                                                   //
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
 * PF 23: Project Detail (Text Form with Diagrams)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

setForm(true);

// Alle Dummy- und Linienfelder anzeigen
enable_dummies();

// Werte ermitteln
if($err)
{
	$fs_methode=$_POST['methode'];
	$fs_zeitpunkt=$_POST['zeitpunkt'];
	$fs_tage=$_POST['tage'];
}
else if(fxIsArray($pinfo))
{
	if($vAPID)
	{
		$fs_methode=$pinfo[$vAPID]['data']['methode'];
		$fs_zeitpunkt=$pinfo[$vAPID]['data']['zeitpunkt'];
		$fs_tage=$pinfo[$vAPID]['data']['tage'];
	}
	else
	{
		$fs_methode=$pinfo[$vEPID]['data']['methode'];
		$fs_zeitpunkt=$pinfo[$vEPID]['data']['zeitpunkt'];
		$fs_tage=$pinfo[$vEPID]['data']['tage'];
	}
}

if(!$fs_methode)
{
	$fs_methode=1784;	// Vergleich Soll-/Ist-Aufwand
	$fs_zeitpunkt=1790;	// Auf Anforderung
	$fs_tage=7;
}
$fs_tage=(int)$fs_tage;
if(!$fs_tage)
	$fs_tage=1;

// Ermittlungmethode + Berichtszeitpunkt
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
{
	$maskenfeldwerte['methode']='~'.meldung($fs_methode, true);
	$maskenfeldwerte['zeitpunkt']='~'.str_replace('[x]', '<b>'.(int)$fs_tage.'</b>', meldung($fs_zeitpunkt, true));
}
else
{
	// Ermittlungmethode (methode)
	$methode_ds=0;
	$maskenfeldwerte['methode']=$fs_methode;

	// Berichtszeitpunkt (zeitpunkt, tage)
	$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=1660 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position, tabwert";
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		$value_array=array();
		foreach($tmp as $t)
		{
			if($t['wertetabid'] == 1792)	// Alle [x] Tage...
				$txt=str_replace('[x]', fieldText('tage',$fs_tage,45,5), $t['tabwert']);
			else
				$txt=$t['tabwert'];

			$value_array[$t['wertetabid']]=$txt;
		}
		$mfz=fieldRadio('zeitpunkt', $fs_zeitpunkt, $value_array);
	}
	else
		$mfz='STANDARD';

	$maskenfeldwerte['zeitpunkt']='~'.$mfz;
}

// Statistiken
if((($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)) && $vAPID && fxIsArray($pinfo) && fxIsArray($pinfo[$vAPID]))
{
	$bwidth=720;
	// ...Soll-/Ist-Vergleich
	$gpercent=$pinfo[$vAPID]['sigrad'] * 100.00;
	$tt=get_text(1676, 'kurz');
	$maskenfeldwerte['fg_sollist']='~'.bar_horizontal($gpercent, $tt, '', $bwidth);
	// ...Anarbeitungsgrad
	$gpercent=$pinfo[$vAPID]['angrad'] * 100.00;
	$tt=get_text(1677, 'kurz');
	$maskenfeldwerte['fg_budget']='~'.bar_horizontal($gpercent, $tt, '', $bwidth);
	// ...Rel. Fertigstellungsgrad
	$gpercent=$pinfo[$vAPID]['rfgrad'] * 100.00;
	$tt=get_text(1678, 'kurz');
	$maskenfeldwerte['fg_relativ']='~'.bar_horizontal($gpercent, $tt, '', $bwidth);
	// ...Abs. Fertigstellungsgrad
	$gpercent=$pinfo[$vAPID]['afgrad'] * 100.00;
	$tt=get_text(1679, 'kurz');
	$maskenfeldwerte['fg_absolut']='~'.bar_horizontal($gpercent, $tt, '', $bwidth);
}
else
{
	$hr_nv=true;
	$fg_sollist_nv=true;
	$fg_budget_nv=true;
	$fg_relativ_nv=true;
	$fg_absolut_nv=true;
}

// Link
if($vAPID && fxIsArray($pinfo) && fxIsArray($pinfo[$vAPID]) && (($pinfo[$vAPID]['data']['projektmanager'] == $GLOBALS['fxpglobals']['person']) || ($pinfo[$vAPID]['data']['projektleiter'] == $GLOBALS['fxpglobals']['person']) || ($pinfo[$vAPID]['data']['vertreter'] == $GLOBALS['fxpglobals']['person'])))
	$ispl=true;
else
	$istpl=false;

$hr5_nv=true;
$link_nv=true;
?>