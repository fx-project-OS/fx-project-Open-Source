<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projektuebersicht_ini.inc                                    //
// Version     : 21.1                                                         //
// Begin       : 2020-12-23                                                   //
// Last Change : 2020-12-23                                                   //
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
 * PF 209: Dashboard
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

//$display_inc='projektuebersicht_disp.inc';

// Direktaufruf mit übergebener Projekt-ID
if($_GET['pid'] || $GLOBALS['gproject'])
{
	if($_GET['pid'])
		$Projekt_ID=$_GET['pid'];
	else
		$Projekt_ID=$GLOBALS['gproject'];
	$Button_Anzeigen=true;
	$show_select=false;
}
// Normaler Transaktionsaufruf
else
{
	if(($_GET['uid'] == 'newtr') && $GLOBALS['fxpglobals']['project'])
		$Projekt_ID=$GLOBALS['fxpglobals']['project'];
	else if($_POST['Projekt_ID'])
		$Projekt_ID=$_POST['Projekt_ID'];
	else if($GLOBALS['fxpglobals']['project'])
		$Projekt_ID=$GLOBALS['fxpglobals']['project'];
	else if ($GLOBALS['fxptdata']['backup']['Projekt_ID'])
		$Projekt_ID=$GLOBALS['fxptdata']['backup']['Projekt_ID'];
	else
		$Projekt_ID=0;
	$show_select=true;
}

// Rechteprüfung
// Auswahlfeld "Vorgang" + Rechteprüfung
$Projekt_ID_arg=array('build'=>true, 'use_actual'=>false, 'with_projects'=>true, 'only_click_allowed'=>true);
$Proj_Arr=auswahl_Projekt_ID(1);
$Projekt_ID_arg=array('override'=>$Proj_Arr);
if($Button_Anzeigen && (!$Projekt_ID || !$Proj_Arr[$Projekt_ID]))
{
	error_msg(2281, false);	// 2281: Sie haben kein Recht diesen Vorgang zu öffnen!
	$Projekt_ID=0;
	$show_select=true;
	$Button_Anzeigen=false;
}

$GLOBALS['fxptdata']['backup']['Projekt_ID']=$Projekt_ID;

// Select mask
if($show_select)
{
	$Projekt_ID_ds=0;
	$maskenfeldwerte['projekt_id']=$Projekt_ID;
	$maske437_sf=true;
	echo(maske(437));
}
$maske437_nv=true;

// Dashboard
if($Button_Anzeigen && $Projekt_ID)
{
	// Do we have ChartDirector?
	$req_fn=fxf_fn_reqFilename('ext_chartdir');
	if(strlen($req_fn))
		require($req_fn);

	$waehrung='&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'];
	$stunden='&nbsp;'.meldung(484);
	$notdefined='<i class="lightgrey">('.meldung(1643, true).')</i>';	// nicht definiert!
	$unbegrenzt='<i class="lightgrey">('.meldung(1678, true).')</i>';	// Unbegrenzt
	$txt_sprungzu=meldung(1697, true);									// Sprung zu
	$mazuordnung=meldung(357, true);									// Mitarbeiterzuordnung

	$pidummy='<img class="ainactive" src="'.$GLOBALS['gfxpath'].'pmnl.png" style="width:18px;height:18px;">';

	// *** Rechte
	$rechte_23	= tr_rechte(23);	// Projekt
	$rechte_110	= tr_rechte(110);	// DMS
	$rechte_43	= tr_rechte(43);	// Resourcenplanung
	$rechte_104	= tr_rechte(104);	// Rechnung
	$rechte_3	= tr_rechte(3);		// Budget

	// *** Textarrays
	$art_array=kategorie_array('projektart');
//fxDebug($art_array, '$art_array');

	// Replacement array
	$ta=array('<'=>"&lt;", '>'=>"&gt;", '"'=>"&quot;");

	// *** Projektinfos holen
	$pinfo=projektinfo($Projekt_ID, false, true, true);
	$pr_info=$pinfo[$Projekt_ID];
	$vn=no_null($pr_info['data']['vorgangsnummer']);
//fxDebug($pr_info, 'Projekt '.$Projekt_ID);

	// *** Untervorgänge ermitteln
	$csqlz=" FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND vorgangsnummer LIKE '".$pr_info['data']['vorgangsnummer']."%'";

	$inf_all="SELECT projekt_id".$csqlz;

	$inf_tp=$inf_all." AND projektart=".FXP_PRJ_SUB;
	$cnt_tp=(int)db_value("SELECT COUNT(projekt_id) AS cnt FROM projekte WHERE projekt_id IN (".$inf_tp.")");
	if(!$cnt_tp)
		$inf_tp="";

	$inf_aufg=$inf_all." AND projektart=".FXP_PRJ_TASK;
	$cnt_aufg=(int)db_value("SELECT COUNT(projekt_id) AS cnt FROM projekte WHERE projekt_id IN (".$inf_aufg.")");
	if(!$cnt_aufg)
		$inf_aufg="";

	$cnt_all=$cnt_tp+$cnt_aufg;
//echo('<HR>$inf_tp ('.$cnt_tp.')="'.$inf_tp.'"<HR>');
//echo('$inf_aufg ('.$cnt_aufg.')="'.$inf_aufg.'"<HR>');
//fxDebug($pr_info,'$pr_info');

	// *** Defaultvariablen für "Abrechenbarer Ist-Aufwand", "Ausgangsrechnungen", "Eingangsrechnungen" + "Relevante Mitarbeiter"
	$ver_ai=0;
	$ara=array('fest'=>array('bezahlt'=>0.0, 'offen'=>0.0, 'rlist_bezahlt'=>'', 'rlist_offen'=>''), 'std'=>array('bezahlt'=>0.0, 'offen'=>0.0, 'rlist_bezahlt'=>'', 'rlist_offen'=>''), 'reise'=>array('bezahlt'=>0.0, 'offen'=>0.0, 'rlist_bezahlt'=>'', 'rlist_offen'=>''), 'mat'=>array('bezahlt'=>0.0, 'offen'=>0.0, 'rlist_bezahlt'=>'', 'rlist_offen'=>''));
	$era=array(3098=>array('bezahlt'=>0.0,'offen'=>0.0,'rlist'=>array()), array('bezahlt'=>0.0,'offen'=>0.0,'rlist'=>array()), array('bezahlt'=>0.0,'offen'=>0.0,'rlist'=>array()));
	$zrpers=array();
	if(strlen($inf_aufg))
	{
		// *** Abrechenbarer Ist-Aufwand
		$asql  = "SELECT SUM(zeitaufwand) AS ai FROM zeitdaten";
		$asql .= " WHERE projekt_id IN (".$inf_aufg.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id>0 AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.")";
		$asql .= " AND (nicht_abrechenbar IS NULL OR nicht_abrechenbar=0)";
		$ver_ai=(int)db_value($asql);
//fxDebug($ver_ai,$asql);

		// *** Bezahlte Ausgangsrechnungen
		$bsql  = "SELECT rechnungs_id, ist_sachmittel, gesamtpreis FROM rechnungsposition";
		$bsql .= " WHERE projekt_id IN (".$inf_all.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($bsql,'*');
//fxDebug($tmp,$bsql);
		if(fxIsArray($tmp))
		{
			$ra=array();
			$ba=array();
			$rcsv='';
			foreach($tmp as $t)
			{
				$rid=(int)$t['rechnungs_id'];
				if(!isset($ba[$rid]))
				{
					$ba[$rid]=array('fest'=>0.0, 'std'=>0.0, 'reise'=>0.0, 'mat'=>0.0);
					if(strlen($rcsv))
						$rcsv .= ',';
					$rcsv .= $rid;
				}

				$rti=(int)$t['ist_sachmittel'];
				$rts='';
				if(($rti == 50) || ($rti == 54))						// Festpreis (Personen + Material)
					$rts='fest';
				else if(($rti == 51) || ($rti == 55) || ($rti == 56))	// Stunden- oder Mengenbasis (Personen + Zeit-Material + Verbrauchs-Material)
					$rts='std';
				else if($rti == 52)										// Reisekosten
					$rts='reise';
				else if($rti == 53)										// Materialkosten (Sofortmaterial)
					$rts='mat';
				else													// Zusatzkosten ohne Projektbezug
					$rts='';

				if(strlen($rts))
					$ba[$rid][$rts] += $t['gesamtpreis'];
			}
//fxDebug($ba,'$ba: $rcsv='.$rcsv);

			$bsql  = "SELECT rechnungs_id, rechnn_f, datum, nettobetrag, ausgegangen_am, bezahlt_am, bezahlt FROM rechnung";
			$bsql .= " WHERE rechnungs_id IN (".$rcsv.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$bsql .= " ORDER BY rechnn_f";
			$tmp=db_values($bsql,'*');
//fxDebug($tmp,$bsql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
				{
					$rid=(int)$t['rechnungs_id'];
					$ra[$rid]=$t;
				}
			}
//fxDebug($ra,'$ra: $rcsv='.$rcsv);

			if(sizeof($ra))
			{
				foreach($ra as $rid => $a)
				{
					if($a['bezahlt'])
						$at='bezahlt';
					else
						$at='offen';

					foreach($ara as $art => $aa)
					{
						if(isset($ba[$rid]) && $ba[$rid][$art])
						{
							$ara[$art][$at] += $ba[$rid][$art];
							$ara[$art]['rlist_'.$at] .= '<tr bgcolor=#ffffff><td><b>'.$a['rechnn_f'].'</b></td><td align=center>'.get_entry($a['datum'],'datum').'</td><td align=center>'.get_entry($a['ausgegangen_am'],'datum').'</td><td align=center>'.get_entry($a['bezahlt_am'],'datum').'</td><td nowrap align=right>'.get_entry($ba[$rid][$art],'dezimal').$waehrung.'</td></tr>';
						}
					}
				}
			}
		}
//fxDebug($ara,'$ara');

		// *** Eingangsrechnungen
		$rsql  = "SELECT rechnungs_id, erechnungstyp, rechnn_f, personen_id, bezahltermin, bezahlt_am, budgettyp, nettobetrag FROM rechnungseingang";
		$rsql .= " WHERE projekt_id IN (".$inf_aufg.") AND (nettobetrag IS NOT NULL AND nettobetrag>0) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($rsql,'*');
//fxDebug($tmp,$rsql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$btyp=(int)$t['budgettyp'];
				$bezahlt=0.0;
				$offen=0.0;
				if(strlen($t['bezahlt_am']))
					$bezahlt=$t['nettobetrag'];
				else
					$offen=$t['nettobetrag'];
				$era[$btyp]['bezahlt'] += $bezahlt;
				$era[$btyp]['offen'] += $offen;
				$era[$btyp]['rlist'][$t['rechnungs_id']]=$t;
			}
		}
//fxDebug($era,'$era');

		// *** Relevante Mitarbeiter
		$zsql  = "SELECT personen_id FROM zeitdaten";
		$zsql .= " WHERE projekt_id IN (".$inf_aufg.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id>0 AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.")";
		$zsql .= " GROUP BY personen_id";
		$tmp=db_values($zsql, '*');
//fxDebug($tmp,$zsql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
				$zrpers[(int)$t['personen_id']]='z';
		}

		$rsql  = "SELECT personen_id FROM maplanung";
		$rsql .= " WHERE projekt_id IN (".$inf_aufg.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id>0";
		$tmp=db_values($rsql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
				$zrpers[(int)$t['personen_id']]='r';
		}
//fxDebug($zrpers, '$zrpers');
	}

	$ma=array();
	$extma=array();
	if(sizeof($zrpers))
	{
		// Personeninfo
		$sql  = "SELECT personen_id, personentyp, pname, vorname, geschlecht";
		$sql .= " FROM personen";
		$sql .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (personen_id IN (".$zsql.") OR personen_id IN (".$rsql."))";
		$sql .= " ORDER BY pname, vorname";
		$tmp=db_values($sql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$persid	= (int)$t['personen_id'];
				$name_l	= trim($t['pname']);
				$name_s	= $name_l;
				$vname	= trim($t['vorname']);
				if(strlen($vname))
				{
					$name_l .= ', '.$vname;
					$name_s .= ', '.substr($vname, 0, 1).'.';
				}
				if($t['personentyp'] == 840)
				{
					$name_s .= ' [E]';
					$extma[$persid]=true;
				}

				if($zrpers[$persid] == 'z')
					$name_s='('.$name_s.')';

				$ma[$persid]=array('name_l'=>$name_l, 'name_s'=>$name_s, 'geschl'=>(int)$t['geschlecht'], 'aufwand_soll'=>0, 'aufwand_ist'=>0, 'ress'=>$zrpers[$persid]);
			}
		}

		// Aufwand-Soll, falls Ressource
		if(fxIsArray($ma))
		{
			$sql  = "SELECT personen_id, SUM(aufwand_std) AS aufwand_soll";
			$sql .= " FROM maplanung";
			$sql .= " WHERE aufwand_std>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id IN (".$rsql.") AND projekt_id IN (".$inf_aufg.")";
			$sql .= " GROUP BY personen_id, projekt_id";
			$tmp =db_values($sql, '*');
//fxDebug($tmp, $sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
					$ma[(int)$t['personen_id']]['aufwand_soll'] += $t['aufwand_soll'];
			}
		}

		// Aufwand-Ist, falls Zeiterfassung
		if(fxIsArray($ma))
		{
			$sql  = "SELECT personen_id, SUM(zeitaufwand) AS aufwand_ist";
			$sql .= " FROM zeitdaten";
			$sql .= " WHERE zeitaufwand>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id IN (".$zsql.") AND projekt_id IN (".$inf_aufg.")";
			$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.")";
			$sql .= " GROUP BY personen_id, projekt_id";
			$tmp =db_values($sql, '*');
//fxDebug($tmp, $sql);
			if(is_array($tmp))
			{
				foreach($tmp as $t)
					$ma[(int)$t['personen_id']]['aufwand_ist'] += $t['aufwand_ist'];
			}
		}
	}
//fxDebug($ma, 'Zugeordnete Mitarbeiter');
//fxDebug($extma, 'Externe Mitarbeiter');

	// Benutzerdefinierte Felder
	prj_userfields();
	$usr_val=false;
	if(sizeof($ufields))
	{
		sessionMask($unumber);
		$usr_sql="SELECT * FROM ".$utable." WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$usr_val=db_values($usr_sql);
//fxDebug($usr_val, $usr_sql);
	}

	$width=1400;
	$cwidth=($width/3)-50;
	$col_width=135;

	$prname=$vn.':&nbsp;';
	$refnr=trim($pr_info['data']['ref_nummer']);
	if(strlen($refnr))
		$prname .= '['.$refnr.']&nbsp;-&nbsp;';
	$prname .= $pr_info['data']['name_projekt'];

	echo('<div id="podet" style="position:relative;left:0;top:0;width:'.$width.'px;height:auto;padding:0;margin:0;white-space:normal;">'.$nl);
	echo('	<div class="maskt" style="border-top:1px solid #ffffff;box-shadow:4px 4px 8px rgba(0,0,0,0.10);"><div class="s4b" style="padding:4px 8px;">'.strtr($prname,$ta).'</div></div>'.$nl);

	// Projektblock
	echo('	<div class="maskc" style="position:absolute;left:0;top:44px;width:31%;height:auto;">'.$nl);
	echo('		<div style="position:absolute;left:50%;top:-12px;width:1px;height:12px;border-left:1px solid #bbbbbb;"></div>'.$nl);
	echo('		<div class="maskt"><div class="s3b grey" style="padding:1px 8px;">'.meldung(327,true).'</div></div>'.$nl);
	echo('		<div class="maski">'.$nl);

	// ...Abschnitt: Info
	echo('			<table width="100%" border="0" cellpadding="1" cellspacing="0">'.$nl);
	// ... + Projektnummer
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top" width="'.$col_width.'"><span class="lit">'.get_text(313, '20').':</span>&nbsp;</td>'.$nl);
	echo('					<td nowrap valign="top">'.$vn.'</td>'.$nl);
	echo('				</tr>'.$nl);
	// ... + PCA-Code
	$pca=$pr_info['data']['pca'];
	if(strlen($pca))
	{
		echo('				<tr>'.$nl);
		echo('					<td nowrap valign="top"><span class="lit">'.get_text(1847, '20').':</span>&nbsp;</td>'.$nl);
		echo('					<td nowrap valign="top">'.strtr($pca,$ta).'</td>'.$nl);
		echo('				</tr>'.$nl);
	}
	// ... + Referenznummer
	if(strlen($refnr))
	{
		echo('				<tr>'.$nl);
		echo('					<td nowrap valign="top"><span class="lit">'.get_text(1342, '20').':</span>&nbsp;</td>'.$nl);
		echo('					<td nowrap valign="top">'.strtr($refnr,$ta).'</td>'.$nl);
		echo('				</tr>'.$nl);
	}
	// ... + Projektbezeichnung
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top"><span class="lit">'.get_text(46, '20').':</span>&nbsp;</td>'.$nl);
	echo('					<td>'.strtr($pr_info['data']['name_projekt'],$ta).'</td>'.$nl);
	echo('				</tr>'.$nl);
	// ... + Mutterprojekt
	if($pr_info['data']['mutterprojekt_id'] && ($pr_info['data']['mutterprojekt_id'] != $Projekt_ID))
	{
		$mpname=db_value("SELECT name_projekt FROM projekte WHERE projekt_id=".$pr_info['data']['mutterprojekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		echo('				<tr>'.$nl);
		echo('					<td nowrap valign="top"><span class="lit">'.get_text(404, '20').':</span>&nbsp;</td>'.$nl);
		echo('					<td>'.$mpname.'</td>'.$nl);
		echo('				</tr>'.$nl);
	}
	// ... + Erstelldatum
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top"><span class="lit">'.get_text(6, '20').':</span>&nbsp;</td>'.$nl);
	echo('					<td nowrap valign="top">'.get_entry($pr_info['data']['erstelldatum'], 'datetime').'</td>'.$nl);
	echo('				</tr>'.$nl);
	// ... + Projekt-Status
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top"><span class="lit">'.get_text(137, '20').':</span>&nbsp;</td>'.$nl);
	echo('					<td nowrap valign="top">'.meldung($pr_info['data']['projekt_status'], true).'</td>'.$nl);
	echo('				</tr>'.$nl);
	// ... + Priorität
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top"><span class="lit">'.get_text(47, '20').':</span>&nbsp;</td>'.$nl);
	echo('					<td nowrap valign="top">'.meldung($pr_info['data']['prioritaet'], true).'</td>'.$nl);
	echo('				</tr>'.$nl);
	// ... + Deadline
	if($pr_info['data']['deadline'])
		$deadline='<font class="lightred">'.get_entry($pr_info['data']['deadline'], 'datum').'</font>';
	else
		$deadline=$unbegrenzt;
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top"><span class="lit">'.get_text(48, '20').':</span>&nbsp;</td>'.$nl);
	echo('					<td nowrap valign="top">'.$deadline.'</td>'.$nl);
	echo('				</tr>'.$nl);
	// ... + Max. Zeitaufwand
	if($pr_info['data']['max_zeit_aufw'] > 0.00)
	{
		$ua=effortSec2Unit($pr_info['data']['max_zeit_aufw']);
		$maxz='<font class="red"><span id="eff_mte">'.$ua['val2'].'</span>&nbsp;<span id="unit_eff_mte">'.$ua['unit2'].'</span></font>';
	}
	else
		$maxz=$unbegrenzt;
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top"><span class="lit">'.get_text(132, '20').':</span>&nbsp;</td>'.$nl);
	echo('					<td nowrap valign="top">'.$maxz.'</td>'.$nl);
	echo('				</tr>'.$nl);
	echo('		</table>'.$nl);

	// ...Abschnitt: Methode
	section_header('meth', get_text(1659, '10'), true);
	$mhn='Methode'.$pr_info['data']['methode'];
	$zp=meldung($pr_info['data']['zeitpunkt'], true);
	if($pr_info['data']['zeitpunkt'] == 1792)	// Alle [x] Tage nach Soll-Startdatum
		$zp=str_replace('[x]', '['.(int)$pr_info['data']['tage'].']', $zp);
	$onclick=' '.fxf_jsFunction('fxLink', false, 'loader.php?url=progress_methods.inc&m='.$pr_info['data']['methode'].'&s='.$GLOBALS['fxpglobals']['lang']);
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top"><span style="cursor:pointer;border-bottom:1px dotted #888888;"'.$onclick.'>'.meldung($pr_info['data']['methode'], true).'</span></td>'.$nl);
	echo('				</tr>'.$nl);
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top">'.$zp.'</td>'.$nl);
	echo('				</tr>'.$nl);
	echo('		</table>'.$nl);

	// ...Abschnitt: Dokumente
	section_header('doc', meldung(38,true), true);
	$ddoc=array();
	$idoc=array();
	if(strlen($inf_all))
	{
		$sql  = "SELECT";
		$sql .= " d.dok_id, d.dok_originalname, d.dok_art, d.dok_typ, d.dok_kategorie, d.archiv, d.archiv_dtm, d.sperr_kz, d.abgerufen_von, d.elter_art, d.elter, d.angelegt_von, d.anlage_dtm, d.beschreibung, d.dok_recht_id, d.auto_version, d.aenderungs_id AS geaendert_von,";
		$sql .= " v.datei_name, v.dateigroesse, v.version_nr, v.aktiv_kz, v.schlagwoerter, v.ablageort, v.aend_beschr, v.aenderungs_id, v.zeitstempel ";
		$sql .= "FROM";
		$sql .= " d_dokumente d, d_dok_verlauf v ";
		$sql .= "WHERE";
		$sql .= " v.dok_id=d.dok_id AND v.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND d.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " AND d.elter_art=3 AND d.elter IN (".$inf_all.") ";
		$sql .= "ORDER BY";
		$sql .= " d.dok_originalname, v.version_nr";

		$docs=db_values($sql, '*');
		if(fxIsArray($docs))
		{
			$__oss=substr($GLOBALS['fxpglobals']['settings']['dok_archiv'], -1);
			$bit=array(1,2,4,8,16,32,64,128);
			foreach($docs as $dc)
			{
				// Prüfen, ob es die Datei überhaupt noch gibt
				$f_exists=false;
				$filename=$GLOBALS['fxpglobals']['settings']['dok_archiv'].$GLOBALS['fxpglobals']['client'].$__oss.$dc['datei_name'];
				if(file_exists($filename))
					$f_exists=true;
				$dc['fexists']=$f_exists;

				// Rechte prüfen, falls nicht Eigentümer
				if(!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] == 1) || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($GLOBALS['fxpglobals']['user'] == $dc['angelegt_von']) || ($GLOBALS['fxpglobals']['person'] == $dc['geaendert_von']))
					$rechte=array(true, true, true, true, true, true, true, true);
				else
					$rechte=rechte_referenz(38, $dc['dok_id'], $GLOBALS['fxpglobals']['user'], $GLOBALS['fxpglobals']['client']);
				$dc['rechtea']=$rechte;
				$dc['rechte']=0;
				for($b=0; $b<8; $b++)
				{
					if($dc['rechtea'][$b])
						$dc['rechte'] |= $bit[$b];
				}

				if($dc['elter'] == $Projekt_ID)
					$ddoc[]=$dc;
				else
					$idoc[]=$dc;
			}
		}
	}
	// ... + Anzahl direkter Dokumente
	$dic='';
	if(fxIsArray($ddoc))
		$dic='<img id="icon_ddoc" src="'.$GLOBALS['gfxpath'].'fl_op.png" style="margin:3px 4px 0 0;cursor:pointer;" '.fxf_jsFunction('toggleSection', 'ddoc').'>';
	echo('				<tr>'.$nl);
	echo('					<td nowrap width="100%">'.$dic.'<span class="lit">'.meldung(1715, true).':</span></td>'.$nl);
	echo('					<td nowrap align="right">'.sizeof($ddoc).'</td>'.$nl);
	echo('				</tr>'.$nl);
	if(fxIsArray($ddoc))
	{
		echo('				<tr>'.$nl);
		echo('					<td id="section_ddoc" colspan="2" style="display:none;">'.$nl);
		echo('						<ul style="padding-left:10px;">'.$nl);
		foreach($ddoc as $dc)
		{
			// Dokument existiert nicht mehr
			if(!$dc['fexists'])
				$dn='<i class="lightgrey">'.$dc['dok_originalname'].'</i>';
			// Keine Downloadberechtigung auf das Dokument
			else if(!$dc['rechtea'][1])
				$dn='<font class="lightred">'.$dc['dok_originalname'].'</font>';
			// Dokument ok
			else
				$dn='<font class="black">'.$dc['dok_originalname'].'</font>';
			echo('							<li>'.$dn.'</li>'.$nl);
		}
		echo('						</ul><br />'.$nl);
		echo('					</td>'.$nl);
		echo('				</tr>'.$nl);
	}
	// ... + Anzahl indirekter Dokumente
	$dic='';
	if(fxIsArray($idoc))
		$dic='<img id="icon_idoc" src="'.$GLOBALS['gfxpath'].'fl_op.png" style="margin:3px 4px 0 0;cursor:pointer;" '.fxf_jsFunction('toggleSection', 'idoc').'>';
	echo('				<tr>'.$nl);
	echo('					<td nowrap width="100%">'.$dic.'<span class="lit">'.meldung(1716, true).':</span></td>'.$nl);
	echo('					<td nowrap align="right">'.sizeof($idoc).'</td>'.$nl);
	echo('				</tr>'.$nl);
	if(fxIsArray($idoc))
	{
		$pta=array();
		echo('				<tr>'.$nl);
		echo('					<td id="section_idoc" colspan="2" style="display:none;">'.$nl);
		echo('						<ul style="padding-left:10px;">'.$nl);
		foreach($idoc as $dc)
		{
			if(!isset($pta[$dc['elter']]))
			{
				$sql="SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$dc['elter']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$tmp=db_values($sql);
				$pta[$dc['elter']]=no_null($tmp['vorgangsnummer']).' - '.$tmp['name_projekt'];
			}
			// Dokument existiert nicht mehr
			if(!$dc['fexists'])
				$dn='<i class="lightgrey">'.$dc['dok_originalname'].'</i>';
			// Keine Downloadberechtigung auf das Dokument
			else if(!$dc['rechtea'][1])
				$dn='<font class="lightred">'.$dc['dok_originalname'].'</font>';
			// Dokument ok
			else
				$dn='<font class="black">'.$dc['dok_originalname'].'</font>';
			$dn .= '&nbsp;&nbsp;<i class="s2 lightergrey">('.$pta[$dc['elter']].')</i>';
			echo('							<li>'.$dn.'</li>'.$nl);
		}
		echo('						</ul>'.$nl);
		echo('					</td>'.$nl);
		echo('				</tr>'.$nl);
	}
	echo('			</table>'.$nl);

	$hstyle=' style="color:#666666;background:#e8e8e8;border-bottom:1px solid #dddddd;border-left:1px solid #dddddd;"';
	$lstyle=' style="border-bottom:1px solid #dddddd;"';
	$dstyle=' style="padding:0 2px;border-bottom:1px solid #dddddd;border-left:1px solid #dddddd;"';
	$slstyle=' style="background:#e4e4e4;border-top:1px solid #aaaaaa;"';
	$sdstyle=' style="padding:0 2px;background:#e4e4e4;border-top:1px solid #aaaaaa;border-left:1px solid #bbbbbb;"';

		// ...Abschnitt: Soll-Ist-Vergleich
	section_header('siv', meldung(1681, true), true);
	// ... + Überschrift Soll/Gen/Ist
	echo('				<tr>'.$nl);
	echo('					<td width="116"></td>'.$nl);
	echo('					<td width="92" nowrap align="center"'.$hstyle.'>'.meldung(1679, true).'</td>'.$nl);	// Soll-Werte
	echo('					<td width="92" nowrap align="center"'.$hstyle.'>'.meldung(717, true).'</td>'.$nl);	// Gen-Werte
	echo('					<td width="92" nowrap align="center"'.$hstyle.'>'.meldung(1680, true).'</td>'.$nl);	// Ist-Werte
	echo('				</tr>'.$nl);
	// ... + Zeitaufwand
	$aitt=explode('|', meldung(3027));	// Summe der abrechenbaren Stunden|Summe der nicht abrechenbaren Stunden|Gesamtsumme der abrechenbaren und nicht abrechenbaren Stunden
	$ua0=effortSec2Unit($ver_ai);
	$ai0='<span id="eff_aei">'.$ua0['val2'].'</span>&nbsp;<span id="unit_eff_aei">'.$ua0['unit2'].'</span>';
	$ua1=effortSec2Unit($pr_info['data']['aufwand_ist']-$ver_ai);
	$ai1='<span id="eff_aen">'.$ua1['val2'].'</span>&nbsp;<span id="unit_eff_aen">'.$ua1['unit2'].'</span>';
	$ua2=effortSec2Unit($pr_info['data']['aufwand_ist']);
	$ai2='<span id="eff_aes">'.$ua2['val2'].'</span>&nbsp;<span id="unit_eff_aes">'.$ua2['unit2'].'</span>';
//fxDebug($aitt,'$aitt');
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top"'.$lstyle.'><span class="lit">'.get_text(274, '20').':</span>&nbsp;</td>'.$nl);
	$ua=effortSec2Unit($pr_info['data']['aufwand_soll']);
	echo('					<td nowrap valign="top" align="right"'.$dstyle.'><span id="eff_pes">'.$ua['val2'].'</span>&nbsp;<span id="unit_eff_pes">'.$ua['unit2'].'</span></td>'.$nl);
	echo('					<td nowrap valign="top" align="right"'.$dstyle.'><font class="grey">&infin;'.$stunden.'</font></td>'.$nl);
	echo('					<td nowrap valign="top" align="right"'.$dstyle.'>'.$nl);
	echo('						<div tooltip="'.$aitt[0].': '.get_entry($ua0['sec']/3600,'dezimal').'" style="cursor:help;"><font class="green">'.$ai0.'</font><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></div>'.$nl);
	echo('						<div tooltip="'.$aitt[1].': '.get_entry($ua1['sec']/3600,'dezimal').'" style="cursor:help;"><font class="darkgrey" style="float:left;">+</font>&nbsp;<font class="red">'.$ai1.'</font><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></div>'.$nl);
	echo('						<div tooltip="'.$aitt[2].': '.get_entry($ua2['sec']/3600,'dezimal').'" style="border-top:1px solid #bbbbbb;cursor:help;">'.$ai2.'<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></div>'.$nl);
	echo('					</td>'.$nl);
	echo('				</tr>'.$nl);
	// ... + Zeitspanne von
	if($pr_info['data']['gen_beginn'])
		$gen=get_entry($pr_info['data']['gen_beginn'], 'datum');
	else
		$gen=$unbegrenzt;
	if($pr_info['data']['ist_beginn'])
		$ist=get_entry($pr_info['data']['ist_beginn'], 'datum');
	else
		$ist='<font class="lightgrey">-</font>';
	echo('				<tr>'.$nl);
	echo('					<td nowrap'.$lstyle.'><span class="lit">'.get_text(572, '20').':</span>&nbsp;</td>'.$nl);
	echo('					<td nowrap align="right"'.$dstyle.'>'.get_entry($pr_info['data']['soll_beginn'], 'datum').'</td>'.$nl);
	echo('					<td nowrap align="right"'.$dstyle.'>'.$gen.'</td>'.$nl);
	echo('					<td nowrap align="right"'.$dstyle.'>'.$ist.'</td>'.$nl);
	echo('				</tr>'.$nl);
	// ... + Zeitspanne bis
	if($pr_info['data']['gen_ende'])
		$gen=get_entry($pr_info['data']['gen_ende'], 'datum');
	else
		$gen=$unbegrenzt;
	if($pr_info['data']['ist_ende'])
		$ist=get_entry($pr_info['data']['ist_ende'], 'datum');
	else
		$ist='<font class="lightgrey">-</font>';
	echo('				<tr>'.$nl);
	echo('					<td nowrap'.$lstyle.'><span class="lit">'.get_text(573, '20').':</span>&nbsp;</td>'.$nl);
	echo('					<td nowrap align="right"'.$dstyle.'>'.get_entry($pr_info['data']['soll_ende'], 'datum').'</td>'.$nl);
	echo('					<td nowrap align="right"'.$dstyle.'>'.$gen.'</td>'.$nl);
	echo('					<td nowrap align="right"'.$dstyle.'>'.$ist.'</td>'.$nl);
	echo('				</tr>'.$nl);
	// ... + Abweichung
	$abwsek=$pr_info['data']['aufwand_ist']-$pr_info['data']['aufwand_soll'];
	if($abwsek < (3600*$GLOBALS['fxpglobals']['settings']['mtag']*$GLOBALS['fxpglobals']['settings']['mwoche']))
		$gfxn='1abw2p';
	else if($abwsek < (3600*$GLOBALS['fxpglobals']['settings']['mtag']))
		$gfxn='1abw1p';
	else if($abwsek > (3600*$GLOBALS['fxpglobals']['settings']['mtag']))
		$gfxn='1abw1m';
	elseif($abwsek > (3600*$GLOBALS['fxpglobals']['settings']['mtag']*$GLOBALS['fxpglobals']['settings']['mwoche']))
		$gfxn='1abw2m';
	else
		$gfxn='1abw0';
	$abwgfx='<img src="'.$GLOBALS['gfxpath'].$gfxn.'.png">';
	$ua=effortSec2Unit($abwsek);
	$abweichung='<span id="eff_edi">'.$ua['val2'].'</span>&nbsp;<span id="unit_eff_edi">'.$ua['unit2'].'</span>'.'&nbsp;'.$abwgfx;
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top"><span class="lit">'.get_text(334, '20').':</span>&nbsp;</td>'.$nl);
	echo('					<td nowrap align="right" colspan="3">'.$abweichung.'</td>'.$nl);
	echo('				</tr>'.$nl);
	echo('				<tr>'.$nl);
	echo('					<td colspan="4">&nbsp;</td>'.$nl);
	echo('				</tr>'.$nl);
	// ... + Fertigstellung (Soll/Ist-Vergl.)
	$fgrade=array('sigrad'=>1676, 'angrad'=>1677, 'rfgrad'=>1678, 'afgrad'=>1679);
	foreach($fgrade as $fgfeldbez => $fgfeldnr)
	{
		$fertig=bar_horizontal($pr_info[$fgfeldbez]*100.00, '', '', $cwidth-185, 65, 13);
		echo('				<tr>'.$nl);
		echo('					<td nowrap valign="top"><span class="lit">'.get_text($fgfeldnr, '15').':</span>&nbsp;</td>'.$nl);
		echo('					<td nowrap align="right" colspan="3">'.$fertig.'</td>'.$nl);
		echo('				</tr>'.$nl);
	}
	echo('		</table>'.$nl);

	// ...Abschnitt: Benutzermaske
	if(sizeof($ufields) && $unumber && fxIsArray($GLOBALS['fx_mask'][$unumber]) && fxIsArray($GLOBALS['fx_mfields'][$unumber]))
	{
		$usr_data	= array();
		$val_ok		= 0;
		foreach($ufields as $fname => $fid)
		{
			$fname=strtolower($fname);
			$usr_data[$fname]=array("lit"=>'', "disp"=>'');

			foreach($GLOBALS['fx_mfields'][$unumber] as $fdata)
			{
				$mfid=(int)$fdata['eingabefeld'];
				if($mfid == $fid)
				{
					$mfanz=(int)$fdata['anzeigeart'];
					$mftyp=(int)$fdata['feldtyp'];
					if(!strlen($usr_data[$fname]['lit']) && (strlen($fdata['literal_value'])))
 						$usr_data[$fname]['lit']=$fdata['literal_value'];
					if(!$usr_data[$fname]['typ'] && ($mfanz > 5) && ($mfanz < 9))
					{
						if($mfanz == 8)
							$val=get_db_value($fname, $usr_val[$fname]);
						else
						{
							$val=get_entry($usr_val[$fname], $GLOBALS['_ftypes'][$mftyp]);
							if(strlen($val))
								$val='&nbsp;'.$val;
						}

						if(strlen($val))
						{
							$val_ok++;
							$usr_data[$fname]['disp']=$val;
						}
						else
							$usr_data[$fname]['disp']='&nbsp;'.$notdefined;
					}
				}
			}
		}
//fxDebug($usr_data, 'USER-Data');

		section_header('usr', $GLOBALS['fx_mask'][$unumber]['text_kurz'].' <i class="normal lightgrey">('.$val_ok.'/'.sizeof($usr_data).')</i>', true);
		foreach($usr_data as $udata)
		{
			echo('				<tr>'.$nl);
			echo('					<td nowrap valign="top"><span class="lit">'.$udata['lit'].':</span></td>'.$nl);
			echo('					<td width="100%">'.$udata['disp'].'</td>'.$nl);
			echo('				</tr>'.$nl);
		}
		echo('		</table>'.$nl);
	}

	// ...Abschnitt: Statistikgrafik
	if(isset($GLOBALS['_ext_chartdir']) && strlen($GLOBALS['_ext_chartdir']))
	{
		$heute=substr(fxNow(), 0, 8);
		// Startdatum ermitteln
		$sbdat=substr($pr_info['data']['soll_beginn'], 0, 8);
		$sdate=$sbdat;
		$ibdat='';
		if($pr_info['data']['ist_beginn'])
		{
			$ibdat=substr($pr_info['data']['ist_beginn'], 0, 8);
			$sdate=min($sdate, $ibdat);
		}
		// Enddatum ermitteln
		$sedat=substr($pr_info['data']['soll_ende'], 0, 8);
		$edate=$sedat;
		$iedat='';
		if($pr_info['data']['ist_ende'])
		{
			$iedat=substr($pr_info['data']['ist_ende'], 0, 8);
			$edate=max($edate, $iedat);
		}
		// Start- und Enddatum korrigieren, falls zu grosse Zeitspanne
		$seda=get_datearray_range($sdate, $edate, $pr_info['data']['projekt_status']);
		$sdate=$seda[0];
		$edate=$seda[1];
		$zsdate=time_add($sdate, -1, 'D');
		$zedate=time_add($edate, 1, 'D');
//echo('$zsdate='.$zsdate.', $zedate='.$zedate.'<hr>');

		// Tagesdifferenz
		$tarray	= array();
		$darray	= array();
		$maxtag	= 0;
		$cim	= (int)(time_diff($sdate, $edate, 'd')/80);

		$motage	= array(1=>31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
		$sy		= (int)substr($zsdate, 0, 4);
		$sm		= (int)substr($zsdate, 4, 2);
		$sd		= (int)substr($zsdate, 6, 2);

		$lj=fx_date('L', fx_mktime(12, 0, 0, 1, 1, $sy));
		if($lj)
			$motage[2]=29;
		else
			$motage[2]=28;

		// Tage
		$adate=$zsdate;
		while($adate <= $zedate)
		{
			$tarray[$adate]	= $maxtag;

			$dt='';
			if($adate == $heute)
				$dt=get_entry($adate, 'datum');
			else if(($adate == $sdate) || ($adate == $edate) || ($adate == $sbdat) || ($adate == $sedat) || ($adate == $ibdat) || ($adate == $iedat))
			{
				// Prüfen, ob Datum angezeigt werden soll
				$cs=$maxtag;
				for($ci=0; $ci<$cim; $ci++)
				{
					$cc=$cs-$ci;
					if(($cc >= 0) && strlen($darray['lab'][$cc]))
					{
						$cs=-1;
						break;
					}
				}
				if($cs >= 0)
					$dt=get_entry($adate, 'datum');
			}
			$darray['lab'][$maxtag]	= $dt;

			$darray[0][$maxtag]=0.00;
			$darray[1][$maxtag]=NoValue;

			$sd++;
			if($sd > $motage[$sm])
			{
				$sd=1;
				$sm++;
				if($sm > 12)
				{
					$sm=1;
					$sy++;
					$lj=fx_date('L', fx_mktime(12, 0, 0, 1, 1, $sy));
					if($lj)
						$motage[2]=29;
					else
						$motage[2]=28;
				}
			}

			$adate=substr('0000'.$sy, -4).substr('00'.$sm, -2).substr('00'.$sd, -2);

			$maxtag++;
		}
//echo('$maxtag='.$maxtag.'<hr>');

		// Ist-Verlauf
		if(strlen($inf_aufg))
		{
//echo('$sdate='.$sdate.', $edate='.$edate.'<hr>');
			// Evtl. Startsumme ermitteln und Werte vorbelegen
			$sql  = "SELECT SUM(zeitaufwand) AS startaufwand FROM zeitdaten WHERE projekt_id IN (".$inf_aufg.")";
			$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND (zeitaufwand IS NOT NULL AND zeitaufwand>0) AND ist_beginn<'".$sdate."000000' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sb  =(int)db_value($sql);
//fxDebug($sb, $sql);
			if($sb > 0)
			{
				$sb=$sb/3600.00;
				for($cnt=1; $cnt<$maxtag; $cnt++)
					$darray[0][$cnt]=$sb;
			}

			$sql  = "SELECT zeitaufwand, ist_beginn FROM zeitdaten WHERE projekt_id IN (".$inf_aufg.")";
			$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND (zeitaufwand IS NOT NULL AND zeitaufwand>0)";
			$sql .= " AND (ist_beginn>='".$sdate."000000' AND ist_beginn<='".$edate."240000') AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sql .= " ORDER BY ist_beginn";
			$tmp =db_values($sql, '*');
//fxDebug($tmp, $sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
				{
					$ib=$t['zeitaufwand']/3600.00;
					$dt	= max($sdate, substr($t['ist_beginn'], 0, 8));
					for($cnt=$tarray[$dt]+1; $cnt<$maxtag; $cnt++)
						$darray[0][$cnt] += $ib;
				}
			}
		}
		$darray[0][0]=NoValue;
		$darray[0][$maxtag-1]=NoValue;

		// Soll-Verlauf
		$igb	= $pr_info['data']['aufwand_soll']/3600.00;
		$sbdiff	= fxp_date_diff($zsdate, $sbdat);
		$stage	= 1 + fxp_date_diff($sbdat, $sedat);
		$sdiff	= $igb/$stage;
//echo('$igb='.$igb.', $sbdiff='.$sbdiff.', $stage='.$stage.', $sdiff='.$sdiff.'<hr>');
		for($cnt=$sbdiff; $cnt<$sbdiff+$stage+1; $cnt++)
		{
			if(($cnt >= 0) && ($cnt < $maxtag))
			{
				$iv=min($igb, ($cnt-$sbdiff)*$sdiff);
				$darray[1][$cnt]=$iv;
			}
		}
		$darray[1][$maxtag-1]=NoValue;
//fxDebug($darray, '<B>Startdatum:</B> '.$sdate.', <B>Enddatum:</B> '.$edate, 0);

		$darray['col']=array(0=>0x88009f6b, 0xaa787878);
		$darray['leg']=array(0=>get_text(298, '20'), get_text(297, '20'));	// Aufwand Ist + Aufwand Soll

		$zavergleich=meldung(1689, true);
		$bild=bar_chartdir($darray, FXP_CT_AREA, $cwidth-20, 220, 7, '', 'h');
		if(strlen($bild))
		{
			// Abschnitt: Chart - Zeitaufwandsvergleich (Soll/Ist)
			section_header('chart_zaw', $zavergleich, false);

			echo('				<tr>'.$nl);
			echo('					<td align="center"><br />'.$bild.'</td>'.$nl);
			echo('				</tr>'.$nl);
			echo('			</table>'.$nl);
		}
	}

	echo('		</div>'.$nl);
	echo('	</div>'.$nl);

	// Personenblock
	$lit_width=150;
	echo('	<div class="maskc" style="position:absolute;left:32%;top:44px;width:34%;height:auto;">'.$nl);
	echo('		<div style="position:absolute;left:50%;top:-12px;width:1px;height:12px;border-left:1px solid #bbbbbb;"></div>'.$nl);
	echo('		<div class="maskt"><div class="s3b grey" style="padding:1px 8px;">'.meldung(335,true).'</div></div>'.$nl);
	echo('		<div class="maski">'.$nl);
	// ...Auftraggeber, Ansprechpartner, Projektmanager, Projektleiter + Stellvertr. PL
	echo('		<table width="100%" border="0" cellpadding="1" cellspacing="0">'.$nl);
	$pearray=array(120=>$pr_info['data']['kunde'], 1195=>$pr_info['data']['ansprechpartner'], 1454=>$pr_info['data']['projektmanager'], 121=>$pr_info['data']['projektleiter'], 1386=>$pr_info['data']['vertreter']);
	foreach($pearray as $textid => $persid)
	{
		if($persid)
			$pe_info=get_email_adress($persid, true);
		echo('				<tr>'.$nl);
		echo('					<td nowrap valign="top" width="'.$lit_width.'"><span class="lit">'.get_text($textid, '20').':</span>&nbsp;</td>'.$nl);
		if($persid && is_array($pe_info) && is_array($pe_info[$persid]))
		{
			echo('					<td nowrap valign="top" width="24">'.$pe_info[$persid]['pic'].'</td>'.$nl);
			echo('					<td>'.strtr($pe_info[$persid]['name1'],$ta).'</td>'.$nl);
		}
		else
		{
			echo('					<td nowrap valign="top" width="24">'.$pidummy.'</td>'.$nl);
			echo('					<td>'.$notdefined.'</td>'.$nl);
		}
		echo('				</tr>'.$nl);
	}

	// Budgetgenemigung
	echo('				<tr>'.$nl);
	echo('					<td nowrap width="'.$lit_width.'" valign="top"><span class="lit">'.get_text(1819, '20').':</span>&nbsp;</td>'.$nl);
	// - Automatisch
	if(strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetgen'], 0, 1)) == 'J')
	{
		echo('					<td nowrap valign="top" width="24">'.$pidummy.'</td>'.$nl);
		echo('					<td><i class="lightgrey">('.meldung(1260, true).')</i></td>'.$nl);
	}
	// - Personen
	else
	{
		$bgarr=array();
		// Alle GF ermitteln
		$sql="SELECT personen_id FROM ma_art_faehigkeiten WHERE maart_id=1 AND mafaehigkeit_id=2 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$gfids=get_array($sql, 'personen_id');
		if(fxIsArray($gfids))
		{
			foreach($gfids as $bgpid => $dmy)
				$bgarr[$bgpid]=2;
		}

		// Alle BG ermitteln, welche Recht auf das akt. Projekt haben
		$bgids=prj_get_bg($Projekt_ID);
		if(fxIsArray($bgids))
		{
			foreach($bgids as $bgpid => $dmy)
				$bgarr[$bgpid]=-4;
		}

		$bgfound=false;
		// - Personen anzeigen
		if(fxIsArray($bgarr))
		{
			$ecol=false;
			$bgpids=array_to_csv($bgarr);
			if(strlen($bgpids))
			{
				$sql  = "SELECT personen_id FROM personen WHERE personen_id IN (".$bgpids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " ORDER BY pname, vorname";
				$pers=db_values($sql, '*');
				if(fxIsArray($pers))
				{
					foreach($pers as $p)
					{
						$persid=(int)$p['personen_id'];
						$pe_info=get_email_adress($persid, true);

						if($ecol)
						{
							echo('				</tr>'.$nl);
							echo('				<tr>'.$nl);
							echo('					<td nowrap>&nbsp;</td>'.$nl);
						}
						else
							$ecol=true;

						$name=$pe_info[$persid]['name1'];
						$fco='';
						$fce='';
						if($bgarr[$p['personen_id']] != 2)	// Kein GF
						{
							$fco='<font class="grey">';
							$fce='</font>';
						}

						echo('					<td nowrap valign="top" width="24">'.$pe_info[$persid]['pic'].'</td>'.$nl);
						echo('					<td>'.$fco.strtr($name,$ta).$fce.'</td>'.$nl);

						$bgfound=true;
					}
				}
			}
		}
		// - Ohne
		if(!$bgfound)
		{
			echo('					<td nowrap valign="top" width="24">'.$pidummy.'</td>'.$nl);
			echo('					<td><i class="lightgrey">('.meldung(388, true).')</i></td>'.$nl);
		}
	}
	echo('				</tr>'.$nl);
	echo('			</table>'.$nl);

	// ...Abschnitt: Zusatzprojektverantwortliche
	$sql  = "SELECT mafaehigkeit_id, mafaehigkeit_name FROM ma_faehigkeit";
	$sql .= " WHERE maart_id=2 AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
	$sql .= " ORDER BY mafaehigkeit_name";
	$zpa=db_values($sql, '*');
	if(fxIsArray($zpa))
	{
		$zpr=array();
		$zpp=array();
		foreach($zpa as $ln => $pv)
		{
			$sql  = "SELECT parameter_int FROM projekte_parameter";
			$sql .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projekt_id=".(int)$Projekt_ID." AND parameter_typ=".$pv['mafaehigkeit_id']." AND parameter_var='INT' AND parameter_txt='PV'";
			$persid =(int)db_value($sql);
//fxDebug($persid, $sql);
			if($persid)
			{
				if(!isset($zpp[$persid]))
					$zpp[$persid]=get_email_adress($persid, true);
				$zpr[$ln]=$persid;
			}
		}

		$size='-';
		if(sizeof($zpp))
			$size=sizeof($zpp);
		$headline=get_text(1246, 'text'); // Zusatzverantwortliche Personen
		section_header('zpv', $headline.'&nbsp;<i class="lightgrey normal">('.$size.'/'.sizeof($zpa).')</i>', true);
		foreach($zpa as $ln => $pv)
		{
			if(isset($zpr[$ln]))
			{
				$persid		= $zpr[$ln];
				$pe_info	= $zpp[$persid];
			}
			else
				$persid		= 0;

			echo('				<tr>'.$nl);
			echo('					<td nowrap width="'.$lit_width.'" valign="top"><span class="lit">'.$pv['mafaehigkeit_name'].':</span>&nbsp;</td>'.$nl);
			if($persid && fxIsArray($pe_info) && fxIsArray($pe_info[$persid]))
			{
				echo('					<td nowrap valign="top" width="24">'.$pe_info[$persid]['pic'].'</td>'.$nl);
				echo('					<td>'.strtr($pe_info[$persid]['name1'],$ta).'</td>'.$nl);
			}
			else
			{
				echo('					<td nowrap valign="top" width="24">'.$pidummy.'</td>'.$nl);
				echo('					<td>'.$notdefined.'</td>'.$nl);
			}
			echo('				</tr>'.$nl);
		}
		echo('			</table>'.$nl);
	}
	// ...Abschnitt: Mitarbeiterzuordnung
	$size='-';
	if(fxIsArray($ma))
		$size=sizeof($ma);
	section_header('maz', $mazuordnung.'&nbsp;<i class="lightgrey normal">('.$size.')</i>', true);
	if(fxIsArray($ma))
	{
		$cls_sag		= 'grey';
		$cls_iag		= 'lightgreen';
		$cls_iap		= 'lightblue';
		$maxsize_labels	= 0;
		$pelabels		= array();
		$pedatas		= array();

		$aufw_so		= 0.00;

		// Überschriften: Name, Aufwand, Soll, Ist
		echo('				<tr>'.$nl);
		echo('					<td width="100%" colspan="2"></td>'.$nl);
		echo('					<td nowrap align="right"'.$hstyle.'>'.meldung(1679, true).'</td>'.$nl);										// Soll-Werte
		echo('					<td nowrap align="right"'.$hstyle.'><font class="'.$cls_sag.'">(%)</font></td>'.$nl);						// (%)
		echo('					<td nowrap align="right"'.$hstyle.'>'.meldung(1680, true).'</td>'.$nl);										// Ist-Werte
		echo('					<td nowrap align="right"'.$hstyle.'><font class="'.$cls_iag.'">(%)</font></td>'.$nl);						// (%)
		echo('					<td nowrap align="right"'.$hstyle.'><font class="'.$cls_iap.'">'.meldung(1696, true).'</font></td>'.$nl);	// S/I
		echo('				</tr>'.$nl);

		foreach($ma as $persid => $madat)
		{
			$pe_info	= get_email_adress($persid, true);
			$name		= $madat['name_s'];

			echo('				<tr>'.$nl);
			// Name
			$clcol='black';
			if($madat['ress'] == 'z')
				$clcol='grey';
			echo('					<td valign="top"'.$lstyle.'>'.$pe_info[$persid]['pic'].'</td>'.$nl);
			echo('					<td valign="bottom"'.$lstyle.'>'.$name.'</td>'.$nl);
			// Soll-Aufwand
			$aufw_s=$madat['aufwand_soll'];
			if(!$aufw_s)
				$aufw_s=0;
			$aufw_so += $aufw_s;
			$ua=effortSec2Unit($aufw_s);
			echo('					<td nowrap valign="bottom" align="right"'.$dstyle.'><span id="eff_rpe_'.$persid.'">'.$ua['val2'].'</span>&nbsp;<span id="unit_eff_rpe_'.$persid.'">'.$ua['unit2'].'</span></td>'.$nl);
			// Soll-Auslastung
			$ausl=0;
			if($pr_info['data']['aufwand_soll'] > 0.00)
				$ausl=round(($aufw_s * 100.00) / $pr_info['data']['aufwand_soll']);
			$pedatas['SAG'][]=$ausl;
			if($ausl >= 100)
				$auslcls='lightgred';
			else
				$auslcls=$cls_sag;
			echo('					<td nowrap valign="bottom" align="right"'.$dstyle.'><font class="'.$auslcls.'">&nbsp;'.get_entry($ausl,'ganzzahl_tz').'%</font></td>'.$nl);
			// Ist-Aufwand
			$aufw_i=$madat['aufwand_ist'];
			if(!$aufw_i)
				$aufw_i=0.00;
			$ua=effortSec2Unit($aufw_i);
			echo('					<td nowrap valign="bottom" align="right"'.$dstyle.'>&nbsp;<span id="eff_rae_'.$persid.'">'.$ua['val2'].'</span>&nbsp;<span id="unit_eff_rae_'.$persid.'">'.$ua['unit2'].'</span></td>'.$nl);
			// Ist-Auslastung (auf Gesamtaufwand gerechnet)
			$ausl=0;
			if($pr_info['data']['aufwand_soll'] > 0.00)
				$ausl=round(($aufw_i * 100.00) / $pr_info['data']['aufwand_soll']);
			$pedatas['IAG'][]=$ausl;
			if($ausl >= 100)
				$auslcls='red';
			else
				$auslcls=$cls_iag;
			echo('					<td nowrap valign="bottom" align="right"'.$dstyle.'><font class="'.$auslcls.'">&nbsp;'.get_entry($ausl,'ganzzahl_tz').'%</font></td>'.$nl);
			// Ist-Auslastung (auf Personenaufwand gerechnet)
			$ausl=0;
			if($aufw_s > 0.00)
				$ausl=round(($aufw_i * 100.00) / $aufw_s);
			$pedatas['IAP'][]=$ausl;
			if($ausl >= 100)
				$auslcls='red';
			else
				$auslcls=$cls_iap;
			echo('					<td nowrap valign="bottom" align="right"'.$dstyle.'><font class="'.$auslcls.'">&nbsp;'.get_entry($ausl,'ganzzahl_tz').'%</font></td>'.$nl);
			echo('				</tr>'.$nl);

			$pelabels[]=$name;
			if(strlen($name) > $maxsize_labels)
				$maxsize_labels=strlen($name);
		}

		// Sind alle Aufgaben zugordnet?
		if($aufw_so < ($pr_info['data']['aufwand_soll']-10))
		{
			$name='N.N.';

			echo('				<tr>'.$nl);
			// Name
			echo('					<td'.$lstyle.'>'.$pidummy.'</td>'.$nl);
			echo('					<td nowrap'.$lstyle.'><font class="grey">'.$name.'</font></td>'.$nl);
			// Soll-Aufwand
			$aufw_s=$pr_info['data']['aufwand_soll']-$aufw_so;
			echo('					<td nowrap align="right"'.$dstyle.'>'.get_entry($aufw_s/3600.00, 'dezimal').$stunden.'</td>'.$nl);
			// Soll-Auslastung
			$ausl=0;
			if($pr_info['data']['aufwand_soll'] > 0.00)
				$ausl=round(($aufw_s * 100.00) / $pr_info['data']['aufwand_soll']);
			$pedatas['SAG'][]=$ausl;
			if($ausl >= 100)
				$auslcls='lightred';
			else
				$auslcls=$cls_sag;
			echo('					<td nowrap align="right"'.$dstyle.'><font class="'.$auslcls.'">'.$ausl.'%</font></td>'.$nl);
			// Ist-Aufwand
			$aufw_i=0.00;
			echo('					<td nowrap align="right"'.$dstyle.'>'.get_entry($aufw_i/3600.00, 'dezimal').$stunden.'</td>'.$nl);
			// Ist-Auslastung (auf Gesamtaufwand gerechnet)
			$ausl=0;
			$pedatas['IAG'][]=$ausl;
			echo('					<td nowrap align="right"'.$dstyle.'><font class="'.$cls_iag.'">'.$ausl.'%</font></td>'.$nl);
			// Ist-Auslastung (auf Personenaufwand gerechnet)
			$ausl=0;
			$pedatas['IAP'][]=$ausl;
			echo('					<td nowrap align="right"'.$dstyle.'><font class="'.$cls_iap.'">'.$ausl.'%</font></td>'.$nl);

			$pelabels[]=$name;
		}
		$pedata_sag	= array_reverse($pedatas['SAG']);
		$pedata_iag	= array_reverse($pedatas['IAG']);
		$pedata_iap	= array_reverse($pedatas['IAP']);
		$pelabels	= array_reverse($pelabels);

		// Statistikgrafik erzeugen
		if(isset($GLOBALS['_ext_chartdir']) && strlen($GLOBALS['_ext_chartdir']))
		{
			$cd_xoffset=$maxsize_labels*5;
			$cd_yoffset=24;
			$cd_width=$cwidth+22;
			$cd_height=$cd_yoffset+26+sizeof($pelabels)*30;

			$c=new XYChart($cd_width, $cd_height);
			$c->setBackground($c->linearGradientColor(0, 0, 0, 100, 0xe0e0e0, 0xffffff), 0xdddddd);
			$c->setRoundedFrame(0xfeffffff);
			$c->setDropShadow(0xfeaaaaaa);

			$c->setPlotArea($cd_xoffset, $cd_yoffset, $cd_width-$cd_xoffset-18, $cd_height-$cd_yoffset-36, 0xfafafa, 0xfcfcfc, 0x888888, 0xc8c8c8, Transparent);

			$c->yAxis->setLabelStyle("arial.ttf", 7, 0x000000);
			$c->yAxis->setTitle(meldung(1717, true), "arialbd.ttf", 8, 0x444444);	// Aufwand in %

			$c->swapXY(true);

			// Ist-Daten-Gesamtaufwand
			$barLayerObj1=$c->addBarLayer($pedata_iag, 0x88009f6b);
			$barLayerObj1->setBorderColor(-1, 1);
			$barLayerObj1->set3D();
			// Ist-Daten-Personenaufwand
			$barLayerObj2=$c->addBarLayer($pedata_iap, 0x99006b9f);
			$barLayerObj2->setBorderColor(-1, 1);
			$barLayerObj2->set3D();
			// Soll-Daten-Gesamtaufwand
			$barLayerObj3=$c->addBarLayer($pedata_sag, 0xaa787878);
			$barLayerObj3->setBorderColor(-1, 1);
			$barLayerObj3->set3D();
			if(fxIsArray($pelabels))
			{
				$trstr=array('ä'=>'ae', 'ö'=>'oe', 'ü'=>'ue', 'ß'=>'ss', 'Ä'=>'Ae', 'Ö'=>'Oe', 'Ü'=>'Ue');
				foreach($pelabels as $lcnt => $ldat)
					$pelabels[$lcnt]=strtr($ldat, $trstr);
				$c->xAxis->setLabelStyle("arial.ttf", 7, 0x000000);
				$c->xAxis->setLabels($pelabels);
			}

			// und als PNG zwischenspeichern
			$img=$c->makeChart2(PNG);
			$fname=saveChartDirectorImage($img, $GLOBALS['tmppath'].'~'.$GLOBALS['datetime'].$GLOBALS['fxpglobals']['person'].'mz.png');
			if(strlen($fname))
			{
				$txt='<img src="'.$fname.'" border="0" width="'.$cd_width.'" height="'.$cd_height.'" title="">';

				// Abschnitt: Chart - Mitarbeiterzuordnung
				echo('			</table>'.$nl);
				section_header('chart_maz', $mazuordnung, false);

				echo('				<tr>'.$nl);
				echo('					<td align="center"><br />'.$txt.'</td>'.$nl);
				echo('				</tr>'.$nl);
			}
		}
	}
	else
	{
		// Keine Mitarbeiter zugeordnet!
		echo('				<tr>'.$nl);
		echo('					<td nowrap valign="top"><i class="lightgrey">('.meldung(1691).')</i></td>'.$nl);
		echo('				</tr>'.$nl);
	}
	echo('			</table>'.$nl);

	echo('		</div>'.$nl);
	echo('	</div>'.$nl);

	// Kostenblock
	$lit_width='117';
	$col_width='102';
	echo('	<div class="maskc" style="position:absolute;right:-10px;top:44px;width:33%;height:auto;">'.$nl);
	echo('		<div style="position:absolute;left:50%;top:-12px;width:1px;height:12px;border-left:1px solid #bbbbbb;"></div>'.$nl);
	echo('		<div class="maskt"><div class="s3b grey" style="padding:1px 8px;">'.get_text(369,'20').'</div></div>'.$nl);
	echo('		<div class="maski">'.$nl);
	// Abschnitt: Art des Budgets
	echo('			<table width="100%" border="0" cellpadding="1" cellspacing="0">'.$nl);
	echo('				<tr>'.$nl);
	echo('					<td nowrap valign="top"><span class="lit">'.get_text(1840, '20').':</span>&nbsp;</td>'.$nl);
	echo('					<td align="right">'.meldung($pr_info['data']['budgettyp'], true).'</td>'.$nl);
	echo('				</tr>'.$nl);
	echo('			</table>'.$nl);
	$kst=darf_kosten_sehen($Projekt_ID);
	if($kst)
	{
		$budgetart	= strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetart'], 0, 1));
		$budgetkat	= array(1754=>'gepl', 1758=>'gen', 1783=>'akt', 1762=>'verpl', 1766=>'verbr');

		if(($kst&1) && (($budgetart == 'I') || ($budgetart == 'B')))	// Internes Budget
		{
			// Abschnitt: Internes Budget (Soll Netto)
			section_header('intbud', get_text(1812, 'text'), true);
			$tdl=' width="'.$lit_width.'"';
			$tdc=' width="'.$col_width.'"';

			// Überschrift Personen/Material/Gesamt
			echo('			<tr>'.$nl);
			echo('				<td></td>'.$nl);
			echo('				<td nowrap align="center"'.$hstyle.'>'.meldung(735, true).'</td>'.$nl);	// Personen
			echo('				<td nowrap align="center"'.$hstyle.'>'.meldung(736, true).'</td>'.$nl);	// Material
			echo('				<td nowrap align="center"'.$hstyle.'>'.meldung(737, true).'</td>'.$nl);	// Gesamt
			echo('			</tr>'.$nl);

			// Rahmen geplant
			if($pr_info['data']['projektart'] <= FXP_PRJ_SUB)
			{
				echo('			<tr>'.$nl);
				echo('				<td nowrap'.$tdl.$lstyle.'><font class="grey">'.get_text(1781, 'text').'</font></td>'.$nl);
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="grey">'.get_entry($pr_info['data']['budgetrahmen_pers_int'], 'dezimal').$waehrung.'</font></td>'.$nl);
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="grey">'.get_entry($pr_info['data']['budgetrahmen_mat_int'], 'dezimal').$waehrung.'</font></td>'.$nl);
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="grey">'.get_entry($pr_info['data']['budgetrahmen_pers_int']+$pr_info['data']['budgetrahmen_mat_int'], 'dezimal').$waehrung.'</font></td>'.$nl);
				echo('			</tr>'.$nl);
				$tdl='';
				$tdc='';
			}

			// Budget geplant, genehmigt, aktiviert, verplant, verbraucht
			$ie='int';
			foreach($budgetkat as $fn => $bt)
			{
				echo('			<tr>'.$nl);
				echo('				<td nowrap'.$tdl.$lstyle.'><span class="lit">'.get_text($fn, 'text').'</span></td>'.$nl);
				$bp=$pr_info['data']['budget_pers_'.$ie.'_'.$bt];
				$cls='darkgrey';
				if($bp < 0.00)
					$cls='lightred';
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="'.$cls.'">'.get_entry($bp, 'dezimal').$waehrung.'</font></td>'.$nl);
				$bm=$pr_info['data']['budget_mat_'.$ie.'_'.$bt];
				$cls='darkgrey';
				if($bm < 0.00)
					$cls='lightred';
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="'.$cls.'">'.get_entry($bm, 'dezimal').$waehrung.'</font></td>'.$nl);
				$bg=$bp+$bm;
				$cls='darkgrey';
				if($bg < 0.00)
					$cls='lightred';
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="'.$cls.'">'.get_entry($bg, 'dezimal').$waehrung.'</font></td>'.$nl);
				echo('			</tr>'.$nl);
				$tdl='';
				$tdc='';
			}

			// Budget Rest
			echo('			<tr>'.$nl);
			echo('				<td nowrap'.$slstyle.'><font class="black">'.get_text(1806, 'text').'</font></td>'.$nl);
			$bp=$pr_info['data']['budget_pers_'.$ie.'_gen']-$pr_info['data']['budget_pers_'.$ie.'_verpl']-$pr_info['data']['budget_pers_'.$ie.'_verbr'];
			$cls='black';
			if($bp < 0.00)
				$cls='red';
			echo('				<td nowrap align="right"'.$sdstyle.'><font class="'.$cls.'">'.get_entry($bp, 'dezimal').$waehrung.'</font></td>'.$nl);
			$bm=$pr_info['data']['budget_mat_'.$ie.'_gen']-$pr_info['data']['budget_mat_'.$ie.'_verpl']-$pr_info['data']['budget_mat_'.$ie.'_verbr'];
			$cls='black';
			if($bm < 0.00)
				$cls='red';
			echo('				<td nowrap align="right"'.$sdstyle.'><font class="'.$cls.'">'.get_entry($bm, 'dezimal').$waehrung.'</font></td>'.$nl);
			$bg=$bp+$bm;
			$cls='black';
			if($bg < 0.00)
				$cls='red';
			echo('				<td nowrap align="right"'.$sdstyle.'><font class="'.$cls.'">'.get_entry($bg, 'dezimal').$waehrung.'</font></td>'.$nl);
			echo('			</tr>'.$nl);
			echo('		</table>'.$nl);
		}

		if(($kst&2) && (($budgetart == 'E') || ($budgetart == 'B')))	// Externes Budget
		{
			// Abschnitt: Externes Budget (Soll Netto)
			section_header('extbud', get_text(1813, 'text'), true);
			$tdl=' width="'.$lit_width.'"';
			$tdc=' width="'.$col_width.'"';

			// Überschrift Personen/Material/Gesamt
			echo('			<tr>'.$nl);
			echo('				<td></td>'.$nl);
			echo('				<td nowrap align="center"'.$hstyle.'>'.meldung(735, true).'</td>'.$nl);	// Personen
			echo('				<td nowrap align="center"'.$hstyle.'>'.meldung(736, true).'</td>'.$nl);	// Material
			echo('				<td nowrap align="center"'.$hstyle.'>'.meldung(737, true).'</td>'.$nl);	// Gesamt
			echo('			</tr>'.$nl);

			// Rahmen geplant
			if($pr_info['data']['projektart'] <= FXP_PRJ_SUB)
			{
				echo('			<tr>'.$nl);
				echo('				<td nowrap'.$tdl.$lstyle.'><font class="grey">'.get_text(1859, 'text').'</font></td>'.$nl);
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="grey">'.get_entry($pr_info['data']['budgetrahmen_pers_ext'], 'dezimal').$waehrung.'</font></td>'.$nl);
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="grey">'.get_entry($pr_info['data']['budgetrahmen_mat_ext'], 'dezimal').$waehrung.'</font></td>'.$nl);
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="grey">'.get_entry($pr_info['data']['budgetrahmen_pers_ext']+$pr_info['data']['budgetrahmen_mat_ext'], 'dezimal').$waehrung.'</font></td>'.$nl);
				echo('			</tr>'.$nl);
				$tdl='';
				$tdc='';
			}

			// Budget geplant, genehmigt, aktiviert, verplant, verbraucht
			$ie='ext';
			$budgetkat[1912]='abger';
			foreach($budgetkat as $fn => $bt)
			{
				$bo='';
				$bc='';
				if($fn == 1912)
				{
					$bo='(';
					$bc=')';
				}

				echo('			<tr>'.$nl);
				echo('				<td nowrap'.$tdl.$lstyle.'><span class="lit">'.get_text($fn+1, 'text').'</span></td>'.$nl);
				$bp=$pr_info['data']['budget_pers_'.$ie.'_'.$bt];
				$cls='darkgrey';
				if($bp < 0.00)
					$cls='lightred';
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="'.$cls.'">'.$bo.get_entry($bp, 'dezimal').$bc.$waehrung.'</font></td>'.$nl);
				$bm=$pr_info['data']['budget_mat_'.$ie.'_'.$bt];
				$cls='darkgrey';
				if($bm < 0.00)
					$cls='lightred';
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="'.$cls.'">'.$bo.get_entry($bm, 'dezimal').$bc.$waehrung.'</font></td>'.$nl);
				$bg=$bp+$bm;
				$cls='darkgrey';
				if($bg < 0.00)
					$cls='lightred';
				echo('				<td nowrap'.$tdc.' align="right"'.$dstyle.'><font class="'.$cls.'">'.$bo.get_entry($bg, 'dezimal').$bc.$waehrung.'</font></td>'.$nl);
				echo('			</tr>'.$nl);
				$tdl='';
				$tdc='';
			}

			// Budget Rest
			echo('			<tr>'.$nl);
			echo('				<td nowrap'.$slstyle.'><font class="black">'.get_text(1807, 'text').'</font></td>'.$nl);
			$bp=$pr_info['data']['budget_pers_'.$ie.'_gen']-$pr_info['data']['budget_pers_'.$ie.'_verpl']-$pr_info['data']['budget_pers_'.$ie.'_verbr'];
			$cls='black';
			if($bp < 0.00)
				$cls='red';
			echo('				<td nowrap align="right"'.$sdstyle.'><font class="'.$cls.'">'.get_entry($bp, 'dezimal').$waehrung.'</font></td>'.$nl);
			$bm=$pr_info['data']['budget_mat_'.$ie.'_gen']-$pr_info['data']['budget_mat_'.$ie.'_verpl']-$pr_info['data']['budget_mat_'.$ie.'_verbr'];
			$cls='black';
			if($bm < 0.00)
				$cls='red';
			echo('				<td nowrap align="right"'.$sdstyle.'><font class="'.$cls.'">'.get_entry($bm, 'dezimal').$waehrung.'</font></td>'.$nl);
			$bg=$bp+$bm;
			$cls='black';
			if($bg < 0.00)
				$cls='red';
			echo('				<td nowrap align="right"'.$sdstyle.'><font class="'.$cls.'">'.get_entry($bg, 'dezimal').$waehrung.'</font></td>'.$nl);
			echo('			</tr>'.$nl);
			echo('		</table>'.$nl);

			if((($kst&3) == 3) && ($budgetart == 'B'))
			{
				// Abschnitt: Aktuelle Marge
				section_header('marge', meldung(1685, true), true);

				// Überschrift Personen/Material/Gesamt
				echo('			<tr>'.$nl);
				echo('				<td></td>'.$nl);
				echo('				<td nowrap align="center"'.$hstyle.'>'.meldung(735, true).'</td>'.$nl);	// Personen
				echo('				<td nowrap align="center"'.$hstyle.'>'.meldung(736, true).'</td>'.$nl);	// Material
				echo('				<td nowrap align="center"'.$hstyle.'>'.meldung(737, true).'</td>'.$nl);	// Gesamt
				echo('			</tr>'.$nl);

				// Margen
				// ...verbraucht
				$bp=$pr_info['data']['budget_pers_ext_verpl']+$pr_info['data']['budget_pers_ext_verbr']-$pr_info['data']['budget_pers_int_verpl']-$pr_info['data']['budget_pers_int_verbr'];
				$bm=$pr_info['data']['budget_mat_ext_verpl']+$pr_info['data']['budget_mat_ext_verbr']-$pr_info['data']['budget_mat_int_verpl']-$pr_info['data']['budget_mat_int_verbr'];
				$bg=$bp+$bm;
				$ic='green';
				if($bg < 0)
					$ic='red';
				echo('			<tr>'.$nl);
				echo('				<td nowrap width="'.$lit_width.'"'.$lstyle.'><img src="'.$GLOBALS['gfxpath'].'tl_'.$ic.'.png">&nbsp;<span class="lit">&Delta;&nbsp;'.substr(get_text(1767, 'text'), 5).'</span></td>'.$nl);
				$cls='grey';
				if($bp < 0.00)
					$cls='lightred';
				echo('				<td nowrap width="'.$col_width.'" align="right"'.$dstyle.'><font class="'.$cls.'">('.get_entry($bp, 'dezimal').')'.$waehrung.'</font></td>'.$nl);
				$cls='grey';
				if($bm < 0.00)
					$cls='lightred';
				echo('				<td nowrap width="'.$col_width.'" align="right"'.$dstyle.'><font class="'.$cls.'">('.get_entry($bm, 'dezimal').')'.$waehrung.'</font></td>'.$nl);
				$cls='grey';
				if($bg < 0.00)
					$cls='lightred';
				echo('				<td nowrap width="'.$col_width.'" align="right"'.$dstyle.'><font class="'.$cls.'">('.get_entry($bg, 'dezimal').')'.$waehrung.'</font></td>'.$nl);
				echo('			</tr>'.$nl);

				// ...abgerechnet
				$bp=$pr_info['data']['budget_pers_ext_abger']-$pr_info['data']['budget_pers_int_verpl']-$pr_info['data']['budget_pers_int_verbr'];
				$bm=$pr_info['data']['budget_mat_ext_abger']-$pr_info['data']['budget_mat_int_verpl']-$pr_info['data']['budget_mat_int_verbr'];
				$bg=$bp+$bm;
				$ic='green';
				if($bg < 0)
					$ic='red';
				echo('			<tr>'.$nl);
				echo('				<td nowrap'.$lstyle.'><img src="'.$GLOBALS['gfxpath'].'tl_'.$ic.'.png">&nbsp;<span class="lit">&Delta;&nbsp;'.substr(get_text(1913, 'text'), 5).'</span></td>'.$nl);
				$cls='black';
				if($bp < 0.00)
					$cls='red';
				echo('				<td nowrap align="right"'.$dstyle.'><font class="'.$cls.'">'.get_entry($bp, 'dezimal').$waehrung.'</font></td>'.$nl);
				$cls='black';
				if($bm < 0.00)
					$cls='red';
				echo('				<td nowrap align="right"'.$dstyle.'><font class="'.$cls.'">'.get_entry($bm, 'dezimal').$waehrung.'</font></td>'.$nl);
				$cls='black';
				if($bg < 0.00)
					$cls='red';
				echo('				<td nowrap align="right"'.$dstyle.'><font class="'.$cls.'">'.get_entry($bg, 'dezimal').$waehrung.'</font></td>'.$nl);
				echo('			</tr>'.$nl);
				echo('		</table>'.$nl);
			}

			// Abschnitt: Ausgangsrechnung (Netto)
			section_header('oinv', meldung(2580,true).' ('.get_text(1072,'5').')', true);

			// Abrechnungsart
			if($pr_info['data']['projektart'] == FXP_PRJ_TASK)
			{
				echo('			<tr>'.$nl);
				echo('				<td nowrap valign="top" colspan="2"><span class="lit">'.get_text(69, 'text').':</span></td>'.$nl);
				echo('				<td nowrap colspan="2" align="right">'.meldung($pr_info['data']['abrechnungsart']).'&nbsp;</td>'.$nl);
				echo('			</tr>'.$nl);
				echo('			<tr>'.$nl);
				echo('				<td nowrap valign="top" colspan="2"><span class="lit">'.get_text(2049, 'text').':</span></td>'.$nl);
				echo('				<td nowrap colspan="2" align="right">'.meldung($pr_info['data']['abrechnungsart_mat']).'&nbsp;</td>'.$nl);
				echo('			</tr>'.$nl);

				echo('			<tr>'.$nl);
				echo('				<td colspan="4" height="4"></td>'.$nl);
				echo('			</tr>'.$nl);
			}

			// Überschrift Abgerechnet/Bezahlt/Offen
			echo('			<tr>'.$nl);
			echo('				<td width="'.$lit_width.'"></td>'.$nl);
			echo('				<td nowrap width="'.($col_width+3).'" align="center"'.$hstyle.'>'.get_text(1826, 'text').'</td>'.$nl);	// Abgerechnet
			echo('				<td nowrap width="'.($col_width+3).'" align="center"'.$hstyle.'>'.meldung(3091).'</td>'.$nl);			// Bezahlt
			echo('				<td nowrap width="'.($col_width+3).'" align="center"'.$hstyle.'>'.get_text(1827, 'text').'</td>'.$nl);	// Offen
			echo('			</tr>'.$nl);

			// FP=Festpreis / SB=Stundenbasis / RK=Reisekosten / MT=Materialkosten
			$th='<table border=0 cellpadding=4 cellspacing=1 bgcolor=#888888><tr><td class=rhl>'.get_text(1048,'10').'</td><td class=rhl align=center>'.get_text(6,'15').'</td><td class=rhl align=center>'.get_text(1079,'15').'</td><td class=rhl align=center>'.get_text(1080,'15').'</td><td class=rhl align=right>'.get_text(1072,'15').'</td></tr>';
			$summe_abger	= 0.0;
			$summe_bezahlt	= 0.0;
			$summe_offen	= 0.0;
			$rechnungskat	= array(50=>'fest', 51=>'std', 52=>'reise', 53=>'mat');
			foreach($rechnungskat as $fn => $rt)
			{
				echo('			<tr>'.$nl);
				echo('				<td nowrap'.$lstyle.'><span class="lit">'.meldung($fn, true).'</span></td>'.$nl);
				$abger=$pr_info['data']['rechnung_'.$rt.'_ext'];
				echo('				<td nowrap align="right"'.$dstyle.'>'.get_entry($abger, 'dezimal').$waehrung.'</td>'.$nl);
				$bezahlt=$ara[$rt]['bezahlt'];
				$tt=$dstyle;
				if(strlen($ara[$rt]['rlist_bezahlt']))
					$tt=substr($tt,0,-1).'cursor:help;" tooltip="'.$th.$ara[$rt]['rlist_bezahlt'].'</table>"><img src="'.$GLOBALS['gfxpath'].'tts.png" align="right"';
				echo('				<td nowrap align="right"'.$tt.'>'.($bezahlt>0.0?'':'<font class="grey">').get_entry($bezahlt, 'dezimal').$waehrung.($bezahlt>0.0?'':'</font>').'</td>'.$nl);
				$offen=$pr_info['data']['rechnung_'.$rt.'_ext_off'];
				echo('				<td nowrap align="right"'.$dstyle.'<font class="grey">'.get_entry($offen, 'dezimal').$waehrung.'</font></td>'.$nl);
				echo('			</tr>'.$nl);

				$summe_abger += $abger;
				$summe_bezahlt += $bezahlt;
				$summe_offen += $offen;
			}

			// Summe
			echo('			<tr>'.$nl);
			echo('				<td nowrap'.$slstyle.'>'.get_text(1005, '5').'</td>'.$nl);
			echo('				<td nowrap align="right"'.$sdstyle.'>'.get_entry($summe_abger, 'dezimal').$waehrung.'</td>'.$nl);
			echo('				<td nowrap align="right"'.$sdstyle.'>'.($summe_bezahlt>0.0?'':'<font class="grey">').get_entry($summe_bezahlt, 'dezimal').$waehrung.($bezahlt>0.0?'':'</font>').'</td>'.$nl);
			echo('				<td nowrap align="right"'.$sdstyle.'><font class="grey">'.get_entry($summe_offen, 'dezimal').$waehrung.'</font></td>'.$nl);
			echo('			</tr>'.$nl);
			echo('		</table>'.$nl);
		}

		if(($kst&1) && (($budgetart == 'I') || ($budgetart == 'B')))	// Internes Budget
		{
			// Abschnitt: Eingangsrechnung (Netto)
			section_header('iinv', meldung(3102,true).' ('.get_text(1072,'5').')', true);

			// Überschrift Bezahlt/Offen
			echo('			<tr>'.$nl);
			echo('				<td width="'.($lit_width+$col_width+3).'"></td>'.$nl);
			echo('				<td nowrap width="'.($col_width+3).'" align="center"'.$hstyle.'>'.meldung(3091).'</td>'.$nl);	// Bezahlt
			echo('				<td nowrap width="'.($col_width+3).'" align="center"'.$hstyle.'>'.meldung(3090).'</td>'.$nl);	// Offen
			echo('			</tr>'.$nl);

			// Personen / Material
			$summe_bezahlt=0.0;
			$summe_offen=0.0;
			$efa=array(3098=>'('.meldung(3098).')', meldung(735), meldung(736));	// Ohne / Personen / Material
			foreach($era as $fn => $rt)
			{
				echo('			<tr>'.$nl);
				echo('				<td nowrap'.$lstyle.'><span class="lit">'.$efa[$fn].'</span></td>'.$nl);
				echo('				<td nowrap align="right"'.$dstyle.'>'.get_entry($rt['bezahlt'], 'dezimal').$waehrung.'</td>'.$nl);
				echo('				<td nowrap align="right"'.$dstyle.'<font class="grey">'.get_entry($rt['offen'], 'dezimal').$waehrung.'</font></td>'.$nl);
				echo('			</tr>'.$nl);

				$summe_bezahlt += $rt['bezahlt'];
				$summe_offen += $rt['offen'];
			}

			// Summe
			echo('			<tr>'.$nl);
			echo('				<td nowrap'.$slstyle.'>'.get_text(1005, '5').'</td>'.$nl);
			echo('				<td nowrap align="right"'.$sdstyle.'>'.get_entry($summe_bezahlt, 'dezimal').$waehrung.'</td>'.$nl);
			echo('				<td nowrap align="right"'.$sdstyle.'><font class="grey">'.get_entry($summe_offen, 'dezimal').$waehrung.'</font></td>'.$nl);
			echo('			</tr>'.$nl);
			echo('		</table>'.$nl);
		}

		// Kuchengrafik erzeugen
		$ie='';
		if(($kst&1) && (($budgetart == 'I') || ($budgetart == 'B')))
			$ie='int';
		else if(($kst&2) && (($budgetart == 'E') || ($budgetart == 'B')))
			$ie='ext';
		if(isset($GLOBALS['_ext_chartdir']) && strlen($GLOBALS['_ext_chartdir']) && strlen($ie))
		{
			$budata=array
			(
				$pr_info['data']['budget_pers_'.$ie.'_akt']-$pr_info['data']['budget_pers_'.$ie.'_verpl']-$pr_info['data']['budget_pers_'.$ie.'_verbr'],
				$pr_info['data']['budget_pers_'.$ie.'_verpl'],
				$pr_info['data']['budget_pers_'.$ie.'_verbr'],
				$pr_info['data']['budget_mat_'.$ie.'_akt']-$pr_info['data']['budget_mat_'.$ie.'_verpl']-$pr_info['data']['budget_mat_'.$ie.'_verbr'],
				$pr_info['data']['budget_mat_'.$ie.'_verpl'],
				$pr_info['data']['budget_mat_'.$ie.'_verbr']
			);
			$bularr=explode('-', meldung(739, true));
			$bulabels=array
			(
				$bularr[0], $bularr[1], $bularr[2],
				$bularr[0], $bularr[1], $bularr[2]
			);

			$cd_width	= $cwidth+8;
			$cd_height	= 16+$cd_width/2;
			$radius		= ($cd_width-16)/5;

			$c=new PieChart($cd_width, $cd_height);
			$c->setBackground($c->linearGradientColor(0, 0, 0, 100, 0xe0e0e0, 0xffffff), 0xaaaaaa);
			$c->setRoundedFrame(0xfeffffff);
			$c->setDropShadow(0xfeaaaaaa);

			$c->setPieSize($cd_width/2, $cd_height/2+8, $radius);

			// Legende: Personen Budget
			$txt=$c->addText($cd_width*1/4, 20, ' '.get_text(1837, 'text').' ', "arialbd.ttf", 8, 0x000000, Center);
			$txt->setBackground(0x79aec7, Transparent, softLighting());
			$txt->setRoundedCorners(5);

			// Legende: Material Budget
			$txt=$c->addText($cd_width*3/4, 20, ' '.get_text(1838, 'text').' ', "arialbd.ttf", 8, 0x000000, Center);
			$txt->setBackground(0x79c7ae, Transparent, softLighting());
			$txt->setRoundedCorners(5);

			$c->set3D(12);
			$c->setSectorStyle(RoundedEdgeShading);
			$c->setLineColor(0x888888);
			$c->setColors2(DataColor, array(0xb5cfde, 0x79aec7, 0x3d8cb3, 0xb5dccf, 0x79c7ae, 0x3db38c));

			$c->setLabelLayout(SideLayout);

			$labelStyleObj=$c->setLabelStyle("arial.ttf", 7, 0x000000);
			$labelStyleObj->setBackground(SameAsMainColor, Transparent, 1);

			$c->setStartAngle(145);
			$c->setData($budata, $bulabels);

			// und als PNG zwischenspeichern
			$img=$c->makeChart2(PNG);
			$fname=saveChartDirectorImage($img, $GLOBALS['tmppath'].'~'.$GLOBALS['datetime'].$GLOBALS['fxpglobals']['person'].'bu.png');
			if(strlen($fname))
			{
				$txt='<img src="'.$fname.'" border="0" width="'.$cd_width.'" height="'.$cd_height.'" title="">';

				// Abschnitt: Chart - Interne/Externe Budgetaufteilung
				if($ie == 'int')
					$titel=meldung(1690, true);	// Interne Budgetaufteilung
				else
					$titel=meldung(738, true);	// Externe Budgetaufteilung
				section_header('chart_bud', $titel, false);
				echo('			<tr>'.$nl);
				echo('				<td align="center"><br />'.$txt.'</td>'.$nl);
				echo('			</tr>'.$nl);
				echo('		</table>'.$nl);
			}
		}
	}
	echo('		</div>'.$nl);
	echo('	</div>'.$nl);
	echo('</div>'.$nl);

	// Standard print icon
	drawPrint('podet', meldung(2585));
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $trn - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function pn_tooltip($trn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dest='';
	if($trn)
		$mn=loadMenu();
	if(fxIsArray($mn))
	{
		foreach($mn as $mnr => $mi)
		{
			if(isset($mi['tr']) && ($mi['tr'] == $trn))
			{
				$dest=$mi['tx'];
				break;
			}
		}
	}
	if(!$dest)
		$dest=$trn;
	if(!$dest)
		$dest='?';

	$tooltip=' tooltip=\"'.$GLOBALS['txt_sprungzu'].': ['.$dest.']';

	return $tooltip;
}

/**
 * ???FUNCTION???
 *
 * @param various $sectionname - Mandatory parameter:                  ???PARAMETER???
 * @param string  $headline    - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $display     - Optional parameter (default = true):  ???PARAMETER???
 */
function section_header($sectionname, $headline='', $display=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $nl;

	if($display)
	{
		$image='fl_cl.png';
		$display='block';
	}
	else
	{
		$image='fl_op.png';
		$display='none';
	}

	echo('		<table width="100%" border="0" cellpadding="3" cellspacing="0" style="margin-top:4px;background:#e1e1e1;border-top:1px solid #ffffff;border-top-left-radius:6px;border-top-right-radius:6px;">'.$nl);
	echo('				<tr>'.$nl);
	echo('					<td nowrap width="100%"><font class="s3b grey">'.$headline.'</font></td>'.$nl);
	echo('					<td nowrap align="right"><img id="icon_'.$sectionname.'" src="'.$GLOBALS['gfxpath'].$image.'" style="margin:3px 4px 0 0;cursor:pointer;" '.fxf_jsFunction('toggleSection', $sectionname).'></td>'.$nl);
	echo('				</tr>'.$nl);
	echo('		</table>'.$nl);
	echo('		<table id="section_'.$sectionname.'" width="100%" border="0" cellpadding="1" cellspacing="0" style="border:1px solid #dddddd;border-top:0;border-bottom-left-radius:4px;border-bottom-right-radius:4px;display:'.$display.';" >'.$nl);
}
?>