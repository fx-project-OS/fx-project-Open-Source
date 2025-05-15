<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_check_pre.inc                                        //
// Version     : 24.1                                                         //
// Begin       : 2020-12-14                                                   //
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
 * 37: Project Checker
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$pinf=array();
if($mpid >= 0)
{
	$iea=array('int', 'ext');
	$barr=getBudgetFieldArray();
	$rarr=getRechnungFieldArray();
}

// Sortierreihenfolge
$sortid=0;
if(fxIsArray($_POST) && isset($_POST['sortierreihe']) && $_POST['sortierreihe'])
	$sortid=1;

$auto=0;
if($_POST['autovn'])
	$auto=1;

if($mpid > 0)
	$pinf[$mpid]=projectCheck($mpid,$iea,$barr,$rarr,'transaction',$sortid,$auto);
else if(fxIsArray($mpa))
{
	foreach($mpa as $id => $pb)
		$pinf[$id]=projectCheck($id,$iea,$barr,$rarr,'transaction',$sortid,$auto);
}
fxDebug($pinf, '$pinf', 2);

// Projektdaten anzeigen
$mpc=sizeof($pinf);
if($mpc)
{
	// Statustexte
	$pstat_arr=fxf_getReferenceValues('projekt_status');
	// Abrechnungsarten
	$abrart_arr=fxf_getReferenceValues('69,2049');

	// Währung ausschalten, da selbst angezeigt wird
	$no_waehrung=true;
	$waehrung=$GLOBALS['fxpglobals']['settings']['waehrung'];

	// Tooltips
	$szr=meldung(1114, true);	// Soll-Zeitraum
	$izr=meldung(1115, true);	// Ist-Zeitraum
	$ttz='~&nbsp;&nbsp;<span tooltip="'.$izr.'" style="cursor:help;">['.strtoupper(substr(trim((string)$izr), 0, 1)).']<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />&nbsp;&nbsp;<span tooltip="'.$szr.'" style="cursor:help;"><font class="grey">['.strtoupper(substr(trim((string)$szr), 0, 1)).']</font><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;';

	$bvl=meldung(1782, true);	// Berechneter Wert
	$dvl=meldung(1783, true);	// Datenbank Wert
	$ttc='~&nbsp;&nbsp;<span tooltip="'.$bvl.'" style="cursor:help;">['.strtoupper(substr(trim((string)$bvl), 0, 1)).']<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />&nbsp;&nbsp;<span tooltip="'.$dvl.'" style="cursor:help;"><font class="grey">['.strtoupper(substr(trim((string)$dvl), 0, 1)).']</font><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;';

	$clst='black';
	$clst2='grey';
	$clsb='blue';
	$clsb2='lightblue';

	$int_sc=substr(meldung(1683, true), 0, 3).'.';	// Int.
	$ext_sc=substr(meldung(1684, true), 0, 3).'.';	// Ext.

	$pbd=get_text(1837, '20');	// Personenbudget
	$mbd=get_text(1838, '20');	// Materialbudget
	$ttpmi  = '~&nbsp;&nbsp;<span tooltip="'.$bvl.'" class="'.$clst.'" style="cursor:help;">'.$int_sc.'&nbsp;'.$pbd.'&nbsp;['.strtoupper(substr(trim((string)$bvl), 0, 1)).']<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />&nbsp;&nbsp;<span tooltip="'.$dvl.'" class="'.$clst2.'" style="cursor:help;">['.strtoupper(substr(trim((string)$dvl), 0, 1)).']</font><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />';
	$ttpmi .= '&nbsp;&nbsp;<span tooltip="'.$bvl.'" class="'.$clsb.'" style="cursor:help;">'.$int_sc.'&nbsp;'.$mbd.'&nbsp;['.strtoupper(substr(trim((string)$bvl), 0, 1)).']<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />&nbsp;&nbsp;<span tooltip="'.$dvl.'" class="'.$clsb2.'" style="cursor:help;">['.strtoupper(substr(trim((string)$dvl), 0, 1)).']</font><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />';
	$ttpme  = '~&nbsp;&nbsp;<span tooltip="'.$bvl.'" class="'.$clst.'" style="cursor:help;">'.$ext_sc.'&nbsp;'.$pbd.'&nbsp;['.strtoupper(substr(trim((string)$bvl), 0, 1)).']<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />&nbsp;&nbsp;<span tooltip="'.$dvl.'" class="'.$clst2.'" style="cursor:help;">['.strtoupper(substr(trim((string)$dvl), 0, 1)).']</font><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />';
	$ttpme .= '&nbsp;&nbsp;<span tooltip="'.$bvl.'" class="'.$clsb.'" style="cursor:help;">'.$ext_sc.'&nbsp;'.$mbd.'&nbsp;['.strtoupper(substr(trim((string)$bvl), 0, 1)).']<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />&nbsp;&nbsp;<span tooltip="'.$dvl.'" class="'.$clsb2.'" style="cursor:help;">['.strtoupper(substr(trim((string)$dvl), 0, 1)).']</font><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />';

	$rad=get_text(1826, 'text');	// Abgerechnet
	$rod=get_text(1827, 'text');	// Offen
	$ttao  = '~&nbsp;&nbsp;<span tooltip="'.$bvl.'" class="'.$clst.'" style="cursor:help;">'.$rad.'&nbsp;['.strtoupper(substr(trim((string)$bvl), 0, 1)).']<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />&nbsp;&nbsp;<span tooltip="'.$dvl.'" class="'.$clst2.'" style="cursor:help;">['.strtoupper(substr(trim((string)$dvl), 0, 1)).']</font><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />';
	$ttao .= '&nbsp;&nbsp;<span tooltip="'.$bvl.'" class="'.$clsb.'" style="cursor:help;">'.$rod.'&nbsp;['.strtoupper(substr(trim((string)$bvl), 0, 1)).']<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />&nbsp;&nbsp;<span tooltip="'.$dvl.'" class="'.$clsb2.'" style="cursor:help;">['.strtoupper(substr(trim((string)$dvl), 0, 1)).']</font><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></span>&nbsp;<br />';

	// NoWrap-Felder
	$Vorgangsnummer_nw			= true;
	$Name_Projekt_nw			= true;
	$Projekt_Status_nw			= true;

	$Zeitraum_nw				= true;

	$Aufwand_Soll_nw			= true;
	$Aufwand_Ist_nw				= true;
	$Ist_Verzoeg_nw				= true;

	$budgetrahmen_pers_int_nw	= true;
	$budget_pers_int_gepl_nw	= true;
	$budget_pers_int_gen_nw		= true;
	$budget_pers_int_akt_nw		= true;
	$budget_pers_int_verpl_nw	= true;
	$budget_pers_int_verbr_nw	= true;

	$budgetrahmen_pers_ext_nw	= true;
	$budget_pers_ext_gepl_nw	= true;
	$budget_pers_ext_gen_nw		= true;
	$budget_pers_ext_akt_nw		= true;
	$budget_pers_ext_verpl_nw	= true;
	$budget_pers_ext_verbr_nw	= true;
	$budget_pers_ext_abger_nw	= true;

	$Abrechnungsart_nw			= true;
	$rechnung_fest_ext_nw		= true;
	$rechnung_std_ext_nw		= true;
	$rechnung_reise_ext_nw		= true;
	$rechnung_mat_ext_nw		= true;

	$Checkbox_nw				= true;

	for($nvt=1; $nvt<6; $nvt++)
		${'nv_text'.$nvt.'_nw'}=true;

	// Je nach ausgewählter Kategorie Spalten ausblenden
	if(!($cat_bits&1))	// Zeitdaten
	{
		$nv_text1_nv				= true;
		$Zeitraum_nv				= true;
	}
	if(!($cat_bits&2))	// Zeitaufwand
	{
		$nv_text2_nv				= true;
		$Aufwand_Soll_nv			= true;
		$Aufwand_Ist_nv				= true;
		$Ist_Verzoeg_nv				= true;
	}
	if(!($cat_bits&4))	// Int. Budget
	{
		$nv_text3_nv				= true;
		$budgetrahmen_pers_int_nv	= true;
		$budget_pers_int_gepl_nv	= true;
		$budget_pers_int_gen_nv		= true;
		$budget_pers_int_akt_nv		= true;
		$budget_pers_int_verpl_nv	= true;
		$budget_pers_int_verbr_nv	= true;
	}
	if(!($cat_bits&8))	// Ext. Budget
	{
		$nv_text4_nv				= true;
		$budgetrahmen_pers_ext_nv	= true;
		$budget_pers_ext_gepl_nv	= true;
		$budget_pers_ext_gen_nv		= true;
		$budget_pers_ext_akt_nv		= true;
		$budget_pers_ext_verpl_nv	= true;
		$budget_pers_ext_verbr_nv	= true;
		$budget_pers_ext_abger_nv	= true;
	}
	if(!($cat_bits&16))	// Abgerechnet
	{
		$nv_text5_nv				= true;
		$Abrechnungsart_nv			= true;
		$rechnung_fest_ext_nv		= true;
		$rechnung_std_ext_nv		= true;
		$rechnung_reise_ext_nv		= true;
		$rechnung_mat_ext_nv		= true;
	}

	// Alle Mutterprojekte
	if(!$mpid)
	{
		$nv_text1_nv				= true;

		$nv_text2_nv				= true;
		$Aufwand_Ist_nv				= true;
		$Ist_Verzoeg_nv				= true;

		$nv_text3_nv				= true;
		$budget_pers_int_gepl_nv	= true;
		$budget_pers_int_gen_nv		= true;
		$budget_pers_int_akt_nv		= true;
		$budget_pers_int_verpl_nv	= true;
		$budget_pers_int_verbr_nv	= true;

		$nv_text4_nv				= true;
		$budget_pers_ext_gepl_nv	= true;
		$budget_pers_ext_gen_nv		= true;
		$budget_pers_ext_akt_nv		= true;
		$budget_pers_ext_verpl_nv	= true;
		$budget_pers_ext_verbr_nv	= true;
		$budget_pers_ext_abger_nv	= true;

		$nv_text5_nv				= true;
		$rechnung_fest_ext_nv		= true;
		$rechnung_std_ext_nv		= true;
		$rechnung_reise_ext_nv		= true;
		$rechnung_mat_ext_nv		= true;

		// Headers
		$Zeitraum_lit=get_text(1292,'10');
		$Aufwand_Soll_lit=get_text(274,'20');
		$budgetrahmen_pers_int_lit=get_text(1816,'text');
		$budgetrahmen_pers_ext_lit=get_text(1817,'text');
		$Abrechnungsart_lit=get_text(1826,'text');

		// Aligns
		$align='right';
		$Zeitraum_align=$align;
		$Aufwand_Soll_align=$align;
		$budgetrahmen_pers_int_align=$align;
		$budgetrahmen_pers_ext_align=$align;
		$Abrechnungsart_align=$align;
	}
	else
	{
		$Abrechnungsart_lit=meldung(3165,false);	// Abrechnungsarten
	}

	$z=0;
	$mc=$GLOBALS['_maskcounter'];

	$mza=array();
	$mca=array('nr'=>array(), 'tp'=>array(), 'zeit'=>array(), 'aufw'=>array(), 'int'=>array(), 'ext'=>array(), 'abger'=>array());

	$tc=array();
	$pc=array();
	$bc=array();
	$zc=array();

	if($mpid)
		fxf_maskColumnBorder(array('nv_text1', 'nv_text2', 'nv_text3', 'nv_text4', 'nv_text5'), 0, -1, $mc);

	foreach($pinf as $mid => $minf)
	{
		$sminf=sizeof($minf);
		foreach($minf as $pid => $p)
		{
			// Projektdaten
			// ... Nr. (Vorgangsnummer)
			$txt=str_repeat('&nbsp;', $p['l']*5);
			$db_vn=$p['p']['vorgangsnummer'];
			$dc_vn=$p['vk'];
			if($auto)
				$dc_vn=$p['vf'];
			$cls='black';
			if($db_vn != $dc_vn)
			{
				$pc[$pid]['vorgangsnummer']="'".$dc_vn."'";
				$cls='red';
				if(!isset($mca['nr'][$mid]))
					$mca['nr'][$mid]=$pid;
				else
					$mca['nr'][$mid] .= ','.$pid;
			}
			if($p['m'] != $p['p']['mutterprojekt_id'])
			{
				$pc[$pid]['mutterprojekt_id']=$p['m'];
				$cls='red';
				if(!isset($mca['nr'][$mid]))
					$mca['nr'][$mid]=$pid;
				else
					$mca['nr'][$mid] .= ','.$pid;
			}
			if($p['e'] != $p['p']['elter'])
			{
				$pc[$pid]['elter']=$p['e'];
				$cls='red';
				if(!isset($mca['nr'][$mid]))
					$mca['nr'][$mid]=$pid;
				else
					$mca['nr'][$mid] .= ','.$pid;
			}
			if($p['p']['projektart'] <= FXP_PRJ_SUB)
				$cls .= ' bold';
			$txt .= '<font class="'.$cls.'">'.no_null($dc_vn).'</font>';
			if($p['m'] != $p['p']['mutterprojekt_id'])
				$txt .= '*';
			if($p['e'] != $p['p']['elter'])
				$txt .= '^';
			if($dc_vn != $p['vd'])
				$txt .= '&nbsp;<br />&nbsp;'.str_repeat('&nbsp;', 8+$p['l']*5).'<font class="grey">('.no_null($p['vd']).')</font>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vorgangsnummer']='~'.$txt;

			// ... Vorgang (Name_Projekt)
			$cls='black';
			if($p['p']['projekt_status'] == FXP_PS_COMPLETED)
				$cls='green';
			$db_pn=$p['p']['name_projekt'];
			$dc_pn=$p['pn'];
			if($db_pn != $dc_pn)
			{
				$pc[$pid]['name_projekt']="'".convert_string($dc_pn,'todb')."'";
				$cls='red';
				if(!isset($mca['nr'][$mid]))
					$mca['nr'][$mid]=$pid;
				else
					$mca['nr'][$mid] .= ','.$pid;
			}
			if($p['p']['projektart'] <= FXP_PRJ_SUB)
				$cls .= ' bold';
			$txt='<font class="'.$cls.'">'.$dc_pn.'</font>';
			if($db_pn != $dc_pn)
				$txt .= '<br /><i class="grey">('.$db_pn.')</i>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt']='~'.$txt;
			// ...Zeiterfassung auf Teilprojekt anpassen
			$stp=sizeof($p['tp']);
			if($stp)
			{
				if($GLOBALS['fxpglobals']['lang'] == 1)
					$stl=array('h'=>"TP-Zeiterfassung in neue Aufgabe umbuchen", 'p'=>"Person", 't'=>"Zeitraum", 'e'=>"Aufwand");
				else
					$stl=array('h'=>"Rebook SP-time-registration to new task", 'p'=>"Person", 't'=>"Timespan", 'e'=>"Effort");

				if($mpid)
					$sp='<br />';
				else
					$sp='&nbsp;&nbsp;';
				if(isset($p['tp'][$pid]))
				{
					$tt ='<table width=100% border=0 cellpadding=2 cellspacing=1 bgcolor=#bbbbbb>';
					$tt .= '<tr><td bgcolor=#eeeeee><b>'.$stl['p'].'</b></td><td align=center bgcolor=#eeeeee><b>'.$stl['t'].'</b></td><td align=right bgcolor=#eeeeee><b>'.$stl['e'].'</b></td></tr>';
					$pct=sizeof($p['tp'][$pid]);
					$ztp=0;
					$zth=0;
					foreach($p['tp'][$pid] as $prs => $zdt)
					{
						$ztp += $zdt['ct'];
						$zth += $zdt['ef'];
						$pdt=get_persondata($prs, $GLOBALS['fxpglobals']['client'], false);
//fxDebug($pdt,$prs);
						$tt .= '<tr><td bgcolor=#fafafa>'.$pdt['name2'].'</td><td align=center bgcolor=#fafafa>'.get_entry($zdt['st'],'datum').'-'.get_entry($zdt['en'],'datum').'</td><td align=right bgcolor=#fafafa>'.get_entry($zdt['ef']/3600,'dezimal').' h</td></tr>';
					}
					$tt .= '</table>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt'] .= $sp.'<b class="red" style="cursor:help;" tooltip="'.$stl['h'].'<br /><br />'.$tt.'">+ '.get_entry($zth/3600,'dezimal').' h ('.$pct.'/'.$ztp.')';
					if(!isset($mca['tp'][$mid]))
						$mca['tp'][$mid]=$pid;
					else
						$mca['tp'][$mid] .= ','.$pid;
				}
				$pct=0;
				$ztp=0;
				$zth=0;
				foreach($p['tp'] as $tpid => $tze)
				{
					if($pid == $tpid)
						$stp--;
					else
					{
						$pct += sizeof($p['tp'][$tpid]);
						foreach($p['tp'][$tpid] as $prs => $zdt)
						{
							$ztp += $zdt['ct'];
							$zth += $zdt['ef'];
						}
					}
				}
				if($pct && $stp && $ztp)
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt'] .= $sp.'<i class="lightred" style="cursor:help;" tooltip="'.$stl['h'].'">+ '.get_entry($zth/3600,'dezimal').' h ('.$pct.'/'.$ztp.'/'.$stp.')';
			}
			if($p['tai'] > 0.00)
			{
/*
				$tc[$pid]['aufwand_ist']=$p['tai'];
				$tc[$pid]['personen_budget_int_vebr']=$p['tibpb'];
				$tc[$pid]['personen_budget_ext_vebr']=$p['tebpb'];
				$tc[$pid]['rechnung_std_ext_off']=$p['trseo'];
				$tc[$pid]['rechnung_reise_ext_off']=$p['trreo'];
*/
			}

			// ... Stat. (Projekt_Status)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt_status']='~'.$pstat_arr[$p['p']['projekt_status']];

			// Zeitdaten
			if($cat_bits&1)
			{
				// ... (nv_text1)
				if($mpid > 0)
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=$ttz;

				// ... Zeitraum (Zeitraum)
				$colib='black';
				if(($p['ib'] === NULL) || (($p['z']['ist_beginn'] !== NULL) && (substr((string)$p['ib'],0,8) != substr((string)$p['z']['ist_beginn'],0,8))))
				{
					if(($p['ib'] === NULL) || !strlen((string)$p['ib']))
						$zc[$pid]['ist_beginn']='NULL';
					else
						$zc[$pid]['ist_beginn']="'".$p['ib']."'";
					$colib='red';
					if(!isset($mca['zeit'][$mid]))
						$mca['zeit'][$mid]=$pid;
					else
						$mca['zeit'][$mid] .= ','.$pid;
				}
				$colie='black';
				if(($p['ie'] === NULL) || (($p['z']['ist_ende'] !== NULL) && (substr((string)$p['ie'],0,8) != substr((string)$p['z']['ist_ende'],0,8))))
				{
					if(($p['ie'] === NULL) || !strlen((string)$p['ie']))
						$zc[$pid]['ist_ende']='NULL';
					else
						$zc[$pid]['ist_ende']="'".$p['ie']."'";
					$colie='red';
					if(!isset($mca['zeit'][$mid]))
						$mca['zeit'][$mid]=$pid;
					else
						$mca['zeit'][$mid] .= ','.$pid;
				}
				$colsb='grey';
				if(($p['sb'] === NULL) || (($p['z']['soll_beginn'] !== NULL) && (substr((string)$p['sb'],0,8) != substr((string)$p['z']['soll_beginn'],0,8))))
				{
					if(($p['sb'] === NULL) || !strlen((string)$p['sb']))
						$zc[$pid]['soll_beginn']='NULL';
					else
						$zc[$pid]['soll_beginn']="'".$p['sb']."'";
					$colsb='red';
					if(!isset($mca['zeit'][$mid]))
						$mca['zeit'][$mid]=$pid;
					else
						$mca['zeit'][$mid] .= ','.$pid;
				}
				$colse='grey';
				if(($p['se'] === NULL) || (($p['z']['soll_ende'] !== NULL) && (substr((string)$p['se'],0,8) != substr((string)$p['z']['soll_ende'],0,8))))
				{
					if(($p['se'] === NULL) || !strlen((string)$p['se']))
						$zc[$pid]['soll_ende']='NULL';
					else
						$zc[$pid]['soll_ende']="'".$p['se']."'";
					$colse='red';
					if(!isset($mca['zeit'][$mid]))
						$mca['zeit'][$mid]=$pid;
					else
						$mca['zeit'][$mid] .= ','.$pid;
				}

				if($mpid > 0)
				{
					$ibd='';
					if($p['ib'])
						$ibd=get_entry($p['ib'], 'datum');
					else if($colib != 'black')
						$ibd='?';
					$ied='';
					if($p['ie'])
						$ied=get_entry($p['ie'], 'datum');
					else if($colie != 'black')
						$ied='?';

					$ist_zr='';
					if(strlen((string)$ibd))
						$ist_zr .= '<font class="'.$colib.'">'.$ibd.'</font>';
					if(strlen((string)$ied) && ($ibd != $ied))
						$ist_zr .= '&nbsp;<font class="grey">-</font>&nbsp;<font class="'.$colie.'">'.$ied.'</font>';
					if(!strlen((string)$ist_zr))
						$ist_zr='<font class="'.$colib.'">?</font>';

					$sbd=get_entry($p['sb'], 'datum');
					$sed=get_entry($p['se'], 'datum');

					$soll_zr='<font class="'.$colsb.'">'.$sbd.'</font>';
					if($sbd != $sed)
						$soll_zr .= '&nbsp;<font class="lightergrey">-</font>&nbsp;<font class="'.$colse.'">'.$sed.'</font>';

					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['zeitraum']='~'.$ist_zr.'<br />'.$soll_zr;
				}
			}

			// Zeitaufwand
			if($cat_bits&2)
			{
				// ... (nv_text2)
				if($mpid > 0)
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text2']=$ttc;

				// Aufw. Soll (Aufwand_Soll)
				$cda=compDouble('aufwand_soll', $p['p']['aufwand_soll']/3600.0, $p['as']/3600.0);
				if($cda['ok'])
				{
					$cls='grey';
					$vz='=&nbsp;';
				}
				else
				{
					$pc[$pid]['aufwand_soll']=(int)($cda['cr']*3600.0);
					$cls='red';
					$vz='&lt;&gt;&nbsp;';
					if(!isset($mca['aufw'][$mid]))
						$mca['aufw'][$mid]=$pid;
					else
						$mca['aufw'][$mid] .= ','.$pid;
				}

				if($mpid > 0)
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aufwand_soll']='~'.$cda['ce'].'&nbsp;h<br /><font class="'.$cls.'">'.$vz.$cda['de'].'&nbsp;h</font>';

				// Aufw. Ist (Aufwand_Ist)
				$cda=compDouble('aufwand_ist', $p['p']['aufwand_ist']/3600.0, $p['ai']/3600.0);
				if($cda['ok'])
				{
					$cls='grey';
					$vz='=&nbsp;';
				}
				else
				{
					$pc[$pid]['aufwand_ist']=(int)($cda['cr']*3600.0);
					$cls='red';
					$vz='&lt;&gt;&nbsp;';
					if(!isset($mca['aufw'][$mid]))
						$mca['aufw'][$mid]=$pid;
					else
						$mca['aufw'][$mid] .= ','.$pid;
				}

				if($mpid > 0)
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aufwand_ist']='~'.$cda['ce'].'&nbsp;h<br /><font class="'.$cls.'">'.$vz.$cda['de'].'&nbsp;h</font>';

				// Abw. (Ist_Verzoeg)
				$cda=compDouble('ist_verzoeg', $p['p']['ist_verzoeg']/3600.0, $p['iv']/3600.0);
				if($cda['ok'])
				{
					$cls='grey';
					$vz='=&nbsp;';
				}
				else
				{
					$pc[$pid]['ist_verzoeg']=(int)($cda['cr']*3600.0);
					$cls='red';
					$vz='&lt;&gt;&nbsp;';
					if(!isset($mca['zeit'][$mid]))
						$mca['zeit'][$mid]=$pid;
					else
						$mca['zeit'][$mid] .= ','.$pid;
				}

				if($mpid > 0)
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ist_verzoeg']='~'.$cda['ce'].'&nbsp;h<br /><font class="'.$cls.'">'.$vz.$cda['de'].'&nbsp;h</font>';
			}

			// Budget
			// ... Intern (nv_text3) + Extern (nv_text4)
			if($mpid > 0)
			{
				if($cat_bits&4)
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text3']=$ttpmi;
				if($cat_bits&8)
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text4']=$ttpme;
			}

			// ... Rahmen Geplant (budgetrahmen_pers_int / budgetrahmen_pers_ext)
			if($cat_bits&12)
			{
				foreach($iea as $ie)
				{
					$ief=substr((string)$ie,0,1);
					if((($cat_bits&4) && ($ief == 'i')) || (($cat_bits&8) && ($ief == 'e')))
						$prjz='*';
					else
						continue;

					if($p['p']['projektart'] <= FXP_PRJ_SUB)
					{
						$prjz='';

						$clp=$clst2;
						$cpa=compDouble('budgetrahmen_pers_'.$ie, $p['b']['budgetrahmen_pers_'.$ie], $p[$ief.'bpr']);
						$vzp='&lt;=&nbsp;';
						if(!$cpa['ok'] && ($cpa['cr'] > $cpa['dr']))
						{
							$bc[$pid]['budgetrahmen_pers_'.$ie]=$cpa['cr'];
							$vzp='&gt;&nbsp;';
							$clp='red';
							if(!isset($mca[$ie][$mid]))
								$mca[$ie][$mid]=$pid;
							else
								$mca[$ie][$mid] .= ','.$pid;
						}

						$clm=$clsb2;
						$cma=compDouble('budgetrahmen_mat_'.$ie, $p['b']['budgetrahmen_mat_'.$ie], $p[$ief.'bmr']);
						$vzm='&lt;=&nbsp;';
						if(!$cma['ok'] && ($cma['cr'] > $cma['dr']))
						{
							$bc[$pid]['budgetrahmen_mat_'.$ie]=$cma['cr'];
							$vzm='&gt;&nbsp;';
							$clm='red';
							if(!isset($mca[$ie][$mid]))
								$mca[$ie][$mid]=$pid;
							else
								$mca[$ie][$mid] .= ','.$pid;
						}

						if($mpid > 0)
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['budgetrahmen_pers_'.$ie]='~<font class="'.$clst.'">'.$cpa['ce'].'&nbsp;'.$waehrung.'</font><br /><font class="'.$clp.'">'.$vzp.$cpa['de'].'&nbsp;'.$waehrung.'</font><br /><font class="'.$clsb.'">'.$cma['ce'].'&nbsp;'.$waehrung.'</font><br /><font class="'.$clm.'">'.$vzm.$cma['de'].'&nbsp;'.$waehrung.'</font>';
					}
				}

				// ... Rest. Budgetfelder
				$prjz='*';
				if($p['p']['projektart'] == FXP_PRJ_TASK)
					$prjz='';
				foreach($barr as $bk => $bt)
				{
					$bk2=substr((string)$bk,2,1);
					if($bk2 == 'm')	// Material
						continue;

					$bk0=substr((string)$bk,0,1);
					if((($cat_bits&4) && ($bk0 == 'i')) || (($cat_bits&8) && ($bk0 == 'e')))
						$obt=$bt;
					else
						continue;

					$clp=$clst2;
					$cpa=compDouble($bt, $p['b'][$bt], $p[$bk]);
					$vzp='=&nbsp;';
					if(!$cpa['ok'])
					{
						$bc[$pid][$bt]=$cpa['cr'];
						$vzp='&lt;&gt;&nbsp;';
						$clp='red';
						if(!isset($mca[$ie][$mid]))
							$mca[$ie][$mid]=$pid;
						else
							$mca[$ie][$mid] .= ','.$pid;
					}

					$bk=substr((string)$bk,0,2).'m'.substr((string)$bk,3);
					$bt=str_replace('_pers_', '_mat_', $bt);

					$clm=$clsb2;
					$cma=compDouble($bt, $p['b'][$bt], $p[$bk]);
					$vzm='=&nbsp;';
					if(!$cma['ok'])
					{
						$bc[$pid][$bt]=$cma['cr'];
						$vzm='&lt;&gt;&nbsp;';
						$clm='red';
						if(!isset($mca[$ie][$mid]))
							$mca[$ie][$mid]=$pid;
						else
							$mca[$ie][$mid] .= ','.$pid;
					}

					if($mpid > 0)
						$GLOBALS['fxptdata']['lvalues'][$mc][$z][$obt]='~<font class="'.$clst.'">'.$cpa['ce'].'&nbsp;'.$waehrung.'</font><br /><font class="'.$clp.'">'.$vzp.$cpa['de'].'&nbsp;'.$waehrung.'</font><br /><font class="'.$clsb.'">'.$cma['ce'].'&nbsp;'.$waehrung.'</font><br /><font class="'.$clm.'">'.$vzm.$cma['de'].'&nbsp;'.$waehrung.'</font>';
				}
			}

			// Rechnung
			if($cat_bits&16)
			{
				// ... (nv_text5)
				if($mpid > 0)
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text5']=$ttao;

				// ... Abrechn. (abrechnungsart)
				if($mpid > 0)
				{
					$pabrtxt=$abrart_arr[$p['b']['abrechnungsart']];
					$mabrtxt=$abrart_arr[$p['b']['abrechnungsart_mat']];
					if($p['p']['projektart'] < FXP_PRJ_TASK)
					{
						$pabrtxt='<i class="lightgrey">('.$pabrtxt.')</i>';
						$mabrtxt='<i class="lightblue">('.$mabrtxt.')</i>';
					}
					else
						$mabrtxt='<span class="blue">'.$mabrtxt.'</span>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['abrechnungsart']='~'.$pabrtxt.'<br /><br />'.$mabrtxt;
				}

				// ... Rechnungsfelder
				$prjz='*';
				if($p['p']['projektart'] == FXP_PRJ_TASK)
					$prjz='';

				foreach($rarr as $rk => $rt)
				{
					$ort=$rt;
					if(substr((string)$rk,-4) == '_off')
						continue;

					$caa=compDouble($rk, $p['b'][$rt], $p[$rk]);
					$cla=$clst2;
					$vza='=&nbsp;';
					if(!$caa['ok'])
					{
						$bc[$pid][$rt]=$caa['cr'];
						$vza='&lt;&gt;&nbsp;';
						$cla='red';
						if(!isset($mca['abger'][$mid]))
							$mca['abger'][$mid]=$pid;
						else
							$mca['abger'][$mid] .= ','.$pid;
					}

					$rk .= 'o';
					$rt .= '_off';

					$clo=$clsb2;
					$coa=compDouble($rk, $p['b'][$rt], $p[$rk]);
					$vzo='=&nbsp;';
					if(!$coa['ok'])
					{
						$bc[$pid][$rt]=$coa['cr'];
						$vzo='&lt;&gt;&nbsp;';
						$clo='red';
						if(!isset($mca['abger'][$mid]))
							$mca['abger'][$mid]=$pid;
						else
							$mca['abger'][$mid] .= ','.$pid;
					}

					if($mpid > 0)
						$GLOBALS['fxptdata']['lvalues'][$mc][$z][$ort]='~<font class="'.$clst.'">'.$caa['ce'].'&nbsp;'.$waehrung.'</font><br /><font class="'.$cla.'">'.$vza.$caa['de'].'&nbsp;'.$waehrung.'</font><br /><font class="'.$clsb.'">'.$coa['ce'].'&nbsp;'.$waehrung.'</font><br /><font class="'.$clo.'">'.$vzo.$coa['de'].'&nbsp;'.$waehrung.'</font>';
				}
			}

			// Chk. (Checkbox)
			if($mpid > 0)
			{
				$img='1';
				if(fxIsArray($tc[$pid]) || fxIsArray($pc[$pid]) || fxIsArray($bc[$pid]) || fxIsArray($zc[$pid]))
					$img='x';
				$img='<img src="'.$GLOBALS['gfxpath'].$img.'.png">';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='~'.$img;
			}

			if($pid == $mid)
			{
				$mza[$mid]=array('n'=>no_null($dc_vn), 'z'=>$z);
				$z++;
			}
			else if(!$mpid)
				unset($GLOBALS['fxptdata']['lvalues'][$mc][$z]);
			else
				$z++;
		}
	}

	// Alle Mutterprojekte
//fxDebug($mza,'$mza', 3);
	if(!$mpid && sizeof($mza))
	{
//fxDebug($mca,'$mca', 0);
		$ca=array('zeit'=>"zeitraum", 'aufw'=>"aufwand_soll", 'int'=>"budgetrahmen_pers_int", 'ext'=>"budgetrahmen_pers_ext", 'abger'=>"abrechnungsart");
		foreach($mza as $mid => $nz)
		{
			$z=$nz['z'];

			$cimg='1';
			// Nr.
			if(isset($mca['nr'][$mid]))
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['vorgangsnummer']='~<b class="red" style="cursor:help;" tooltip="<b>IDs</b>:<br /><br />'.$mca['nr'][$mid].'">'.$nz['n'].'</b>';
				$cimg='x';
			}
			// Projekt (ZE auf Teilprojekt)
			if(isset($mca['tp'][$mid]))
				$cimg='x';
			foreach($ca as $k => $f)
			{
				if(isset($mca[$k][$mid]))
				{
					$c=substr_count($mca[$k][$mid],',');
					$t='<b class="red" style="cursor:help;" tooltip="<b>IDs</b>:<br /><br />'.$mca[$k][$mid].'">'.($c+1).'</b>';
					$cimg='x';
				}
				else
					$t='<font class="lightergrey">0</font>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z][$f]='~'.$t;
			}
			// Chk. (Checkbox)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='~<img src="'.$GLOBALS['gfxpath'].$cimg.'.png">';
		}
	}

	// Leerzeile mit Top Border?
	if($z)
	{
		if($mpid)
		{
			$fa=array(
				'Vorgangsnummer', 'Name_Projekt', 'Projekt_Status',
				'nv_text1|0', 'Zeitraum',
				'nv_text2|0', 'Aufwand_Soll', 'Aufwand_Ist', 'Ist_Verzoeg',
				'nv_text3|0', 'budgetrahmen_pers_int', 'budget_pers_int_gepl', 'budget_pers_int_gen', 'budget_pers_int_akt', 'budget_pers_int_verpl', 'budget_pers_int_verbr',
				'nv_text4|0', 'budgetrahmen_pers_ext', 'budget_pers_ext_gepl', 'budget_pers_ext_gen', 'budget_pers_ext_akt', 'budget_pers_ext_verpl', 'budget_pers_ext_verbr', 'budget_pers_ext_abger', 'Abrechnungsart',
				'nv_text5|0', 'rechnung_fest_ext', 'rechnung_std_ext', 'rechnung_reise_ext', 'rechnung_mat_ext', 'Checkbox'
			);
		}
		else
		{
			$z++;
			$fa=array(
				'Vorgangsnummer', 'Name_Projekt', 'Projekt_Status', 'Zeitraum', 'Aufwand_Soll', 'budgetrahmen_pers_int', 'budgetrahmen_pers_ext', 'Abrechnungsart', 'Checkbox'
			);
		}
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}

	// Keine Änderung notwendig, dann Button ausblenden...
	if(!sizeof($tc) && !sizeof($pc) && !sizeof($bc) && !sizeof($zc))
		$Button_Anpassen_nv=true;
	// ...sonst Änderungen merken für DB
	else
	{
		$GLOBALS['fxptdata']['backup']['tc']=$tc;
		$GLOBALS['fxptdata']['backup']['pc']=$pc;
		$GLOBALS['fxptdata']['backup']['bc']=$bc;
		$GLOBALS['fxptdata']['backup']['zc']=$zc;
//fxDebug($GLOBALS['fxptdata']['backup'], 'backup');
	}
}
else
	$Button_Anpassen_nv=true;
?>