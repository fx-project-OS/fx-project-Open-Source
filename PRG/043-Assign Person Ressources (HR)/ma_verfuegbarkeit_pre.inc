<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ma_verfuegbarkeit_pre.inc                                    //
// Version     : 21.2                                                         //
// Begin       : 2020-12-10                                                   //
// Last Change : 2020-12-10                                                   //
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
 * PF 43: Assign Person Ressources (HR)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$mskfilter=0;

// Variablen
$projekt_art_59=meldung(585);		// Projekt
$projekt_art_60=meldung(582);		// Teilprojekt
$projekt_art_61=meldung(621);		// Aufgabe

$personentyparray=array(275=>get_text(1225, '5'), 840=>get_text(1229, '5'));	// Int. / Ext.

$evzp_txt=explode('|', meldung(2892, false));	// Neuen EK/VK zuordnen|Ressourcen EK/VK Zuordnung|Auswahlmöglichkeiten|EK|VK|Aktueller EK/VK|Standard Personen EK/VK|Kalkulierter EK/VK|Zugeordneter EK/VK
$close_literal=meldung(968, false);				// Schließen

$btype=strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetart'],0,1));	// [N]=None, [I]=Intern, [E]=Extern or [B]=Both
$cst=darf_kosten_sehen();														// [0]=None, [1]=Intern, [2]=Extern or [3]=Both
if(!$cst || ($btype == 'N') || (($cst == 1) && ($btype == 'E')) || (($cst == 2) && ($btype == 'I')))
	$budgetart='N';
else if($cst == 1)
	$budgetart='I';
else if($cst == 2)
	$budgetart='E';
else
	$budgetart=$btype;
//fxDebug($budgetart,'$budgetart: $btype='.$btype.', $cst='.$cst);

// Dummy-Icons
$eva=array('ek', 'vk');
$ieva=array('ok', 'kl', 'gr', 'ge', 'dt');
echo('<div style="position:absolute;display:none;">');
foreach($eva as $ev)
{
	$txt=explode('|', str_replace('$var1', $pb_string[$ev], meldung(2893, false)));	// Aktueller $var1 entspricht dem Standard-Personen-$var1|Aktueller $var1 ist kleiner als der Standard-Personen-$var1|Aktueller $var1 ist größer als der Standard-Personen-$var1|Aktueller $var1 entspricht dem kalkulierten $var1 der Aufgaben|Aktueller $var1 entspricht dem Durchschnitts-$var1 der zugeordneten Aufgaben

	foreach($ieva as $ievn => $iev)
	{
		${'i'.$ev.$iev}='<img src="'.$GLOBALS['gfxpath'].'rp'.$iev.'.png" hspace="2" vspace="0" tooltip="'.$txt[$ievn].'" style="cursor:help;">';
		echo('<span id="i'.$ev.$iev.'">'.${'i'.$ev.$iev}.'</span>');
	}
}
echo('</div>'.$nl);

if(!$awerr && !$_POST['Button_Anzeigen'] && !$_POST['MAZ'])
	echo('<div id="first_assign" style="position:absolute;display:none;">1</div>'.$nl);

// Alle Fähigkeiten der Aufgaben ermitteln
$sql="SELECT projekt_id, maart_id, mafaehigkeit_id FROM projekte WHERE maart_id>0 AND projekt_id>0 AND projektart=".FXP_PRJ_TASK." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
if(fxIsArray($tmp))
{
	$task_skills='';
	foreach($tmp as $t)
	{
		if(strlen($task_skills))
			$task_skills .= ',';
		$task_skills .= $t['projekt_id'].','.(int)$t['maart_id'].','.(int)$t['mafaehigkeit_id'];
	}
	echo('<div id="task_skills" style="position:absolute;display:none;">'.$task_skills.'</div>');
}

// Vorgang (Projekt_ID)
$maskenfeldwerte['projekt_id']=$Projekt_ID;
if(fxIsArray($spa))
	$Projekt_ID_arg['override']=$spa;

// Abteilung (Abt_ID)
$api=array();
$abt=(int)$_POST['Abt_ID'];
$maskenfeldwerte['abt_id']=$abt;
if($abt)
{
	$abt_csv=inferior_abteilung($abt);
//fxDebug($abt_csv, $abt);
	$sql="SELECT personen_id, abt_id FROM personen WHERE abt_id IN (".$abt_csv.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$api[$t['personen_id']]=$t['abt_id'];
	}
}

// Person (Personen_ID)
$pers_zug=array();
$maskenfeldwerte['personen_id']=$Personen_ID;
if($con || $GLOBALS['fxpglobals']['persdat']['roles'][-3] || $GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2])
	$ignore_personenrechte=true;
$Pers_Arr=auswahl_Personen_ID(1);
if(sizeof($Pers_Arr) > 1)
	$Personen_ID_ds=1410;	// Alle Personen
else if(sizeof($Pers_Arr) == 1)
	$Personen_ID_ds=0;	// Eine Persone
else
	$Personen_ID_ds=2144;	// Keiner
$Personen_ID_arg=array('override'=>$Pers_Arr);

// ...Personen-IDs ermitteln
$pers_ids='';
if(fxIsArray($Personen_ID))
{
	if(!$Personen_ID[0])	// Alle Personen
	{
		if(fxIsArray($Pers_Arr))
		{
			foreach($Pers_Arr as $p => $pname)
			{
				if($p && (!sizeof($api) || isset($api[$p])))
				{
					if($pers_ids)
						$pers_ids .= ',';
					$pers_ids .= (int)$p;
				}
			}
		}
	}
	else
	{
		foreach($Personen_ID as $p)
		{
			if($p && (!sizeof($api) || isset($api[$p])))
			{
				if($pers_ids)
					$pers_ids .= ',';
				$pers_ids .= (int)$p;
			}
		}
	}
}
else if($Personen_ID)			// Eine Person
{
	if(!sizeof($api) || isset($api[$Personen_ID]))
		$pers_ids=$Personen_ID;
}
else							// Alle Personen
{
	if(fxIsArray($Pers_Arr))
	{
		foreach($Pers_Arr as $p => $pname)
		{
			if($p && (!sizeof($api) || isset($api[$p])))
			{
				if($pers_ids)
					$pers_ids .= ',';
				$pers_ids .= (int)$p;
			}
		}
	}
}

// Falls "Nur zugeordnete Personen" angehakt ist, dann Personen-ID auf Dummywert setzen
if($_POST['nur_zugeordnet'] || !strlen($pers_ids))
	$pers_ids=-1;

// Fähigkeit (MaFaehigkeit_ID)
$fgr_ids='';
$fae_ids='';
if(isset($_POST) && isset($_POST['MaFaehigkeit_ID']) && fxIsArray($_POST['MaFaehigkeit_ID']))
{
	if($_POST['MaFaehigkeit_ID'][0] || (sizeof($_POST['MaFaehigkeit_ID']) > 1))
	{
		foreach($_POST['MaFaehigkeit_ID'] as $fae)
		{
			if(strlen($fae) && (int)$fae)
			{
				if(substr($fae, -1) == 'f')
				{
					if(strlen($fae_ids))
						$fae_ids .= ',';
					$fae_ids .= (int)$fae;
				}
				else
				{
					if(strlen($fgr_ids))
						$fgr_ids .= ',';
					$fgr_ids .= (int)$fae;
				}
			}
		}
	}
}
//echo('<b>$fgr_ids</b>: '.$fgr_ids.'<br>'.$nl);
//echo('<b>$fae_ids</b>: '.$fae_ids.'<br>'.$nl);
$maskenfeldwerte['mafaehigkeit_id']=$MaFaehigkeit_ID;

// Auf jeden Fall immer alle Mitarbeiter anzeigen, die bereits zu Aufgaben zugeordnet sind
if($assign_granted && $iau)
{
	$sql="SELECT personen_id, projekt_id FROM maplanung WHERE projekt_id IN (".$iau.") AND (personen_id IS NOT NULL AND personen_id>0) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$pid=(int)$t['personen_id'];
			if(!isset($pers_zug[$pid]))
			{
				if($pers_ids)
					$pers_ids .= ',';
				$pers_ids .= $pid;
			}
			$pers_zug[$pid][(int)$t['projekt_id']]=true;
		}
	}
}

if(strlen($pers_ids))
{
	$pers_sql ="SELECT p.personen_id, p.mandanten_id, p.pname, p.vorname, p.personentyp, p.geschlecht";
	$pers_sql .= " FROM personen p";
	$pers_sql .= " WHERE p.personen_id IN (".$pers_ids.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$pers_sql .= " ORDER BY p.mandanten_id, p.pname, p.vorname";
	if($GLOBALS['__DGBLVL'] == 2)
		fxDebug($pers_sql, 'SQL zum Auslesen der Mitarbeiter');
	$pers_arr=db_values($pers_sql, '*');
}
else
	$pers_arr='';

if($GLOBALS['__DGBLVL'] == 3)
{
	fxDebug($pers_arr, '$pers_sql<br>'.$pers_sql);
	fxDebug($pers_zug, '$pers_zug<br>'.$pers_sql2);
}

// Alle MaDaten einladen
$multimd=false;
$mand_max=0;
$pers_ids='';
$madaten=array();
if(fxIsArray($pers_arr))
{
	foreach($pers_arr as $p)
	{
		if($pers_ids)
			$pers_ids .= ',';
		$pers_ids .= (int)$p['personen_id'];

		$mid=(int)$p['mandanten_id'];
		if(!$mand_max)
			$mand_max=$mid;
		else if($mand_max != $mid)
		{
			$multimd=true;
			if($mid > $mand_max)
				$mand_max=$mid;
		}
	}

	if($_POST['Button_Anzeigen'] && fxIsArray($pers_arr) && !$fehler && ($awerr < 2))
	{
		$madsql  = "SELECT";
		$madsql .= " mandanten_id, personen_id, arbeitstage, wochenarbeitstd, maeintrittsdatum, maaustrittsdatum, soll_beginn AS startzeit, pause,";
		$madsql .= " sonntag AS a, montag AS b, dienstag AS c, mittwoch AS d, donnerstag AS e, freitag AS f, samstag AS g ";
		$madsql .= "FROM";
		$madsql .= " madaten ";
		$madsql .= "WHERE";
		$madsql .= " personen_id IN (".$pers_ids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($madsql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
				$madaten[(int)$t['personen_id']]=$t;
		}
	}
}

// Felder "Zeitspanne_von" + "Zeitspanne_bis"
if($von)
	$von=substr($von, 0, 8).'000000';
$maskenfeldwerte['zeitspanne_von']=$von;
if($bis)
	$bis=substr($bis, 0, 8).'240000';
$maskenfeldwerte['zeitspanne_bis']=$bis;

// Felder "ZZeitProjekt", "nur_plandaten", "nur_zugeordnet", "ZAuslastung" + "ZProjekte"
if(($_GET['nid'] || $_GET['akfr']) && !$GLOBALS['fxptdata']['backup'])
	$_POST['ZAuslastung']=1;
$maskenfeldwerte['zzeitprojekt']=$_POST['ZZeitProjekt'];
$maskenfeldwerte['nur_plandaten']=$_POST['nur_plandaten'];
$maskenfeldwerte['nur_zugeordnet']=$_POST['nur_zugeordnet'];
$maskenfeldwerte['zauslastung']=$_POST['ZAuslastung'];
$maskenfeldwerte['zprojekte']=$_POST['ZProjekte'];

// Felder "atyp_akt", "atyp_gen" + "atyp_gepl"
$atyp_akt_lit='<font style="padding:0 4px;margin:1px;border-right:1px solid #444444;border-bottom:1px solid #444444;color:#000000;background:#ffffff;border-radius:4px;">'.get_text(1906, 'text').'</font>';
$atyp_gen_lit='<font style="padding:0 4px;margin:1px;border-right:1px solid #444444;border-bottom:1px solid #444444;color:#005077;background:#ffffff;border-radius:4px;">'.get_text(1908, 'text').'</font>';
$atyp_gepl_lit='<font style="padding:0 4px;margin:1px;border-right:1px solid #444444;border-bottom:1px solid #444444;color:#787878;background:#ffffff;border-radius:4px;">'.get_text(1907, 'text').'</font>';
if(($_GET['nid'] || $_GET['akfr']) && !$GLOBALS['fxptdata']['backup'])
{
	$_POST['atyp_akt']=1;
	$_POST['atyp_gen']=1;
}
$maskenfeldwerte['atyp_akt']=$_POST['atyp_akt'];
$maskenfeldwerte['atyp_gen']=$_POST['atyp_gen'];
$maskenfeldwerte['atyp_gepl']=$_POST['atyp_gepl'];

// Feld "wartung_anz"
$maskenfeldwerte['wartung_anz']	= $_POST['wartung_anz'];

// Dateimanager nur anzeigen, falls Auslastung oder Projekte angezeigt werden sollen und keine Zuordnung durchgeführt werden soll
if($assign_granted || (!$_POST['ZAuslastung'] && !$_POST['ZProjekte']))
	$hide_dm=true;
else
	$hide_dm=false;

// Automatischer Kalenderanzeigemodus
if(!$Kalenderanzeigemodus || ($Kalenderanzeigemodus == 1260))
{
	$Kalenderanzeigemodus=1260;
	$ke='A';
}
// Je nach Kalenderanzeigemodus die Darstellungsart festlegen
else
{
	$ke_array=array(1255=>'D', 1256=>'W', 1257=>'M', 1258=>'Y');
	$ke=$ke_array[$Kalenderanzeigemodus];
}
$Kalenderanzeigemodus_ds=1260;	// Automatisch
$maskenfeldwerte['kalenderanzeigemodus']=$Kalenderanzeigemodus;

// Mitarbeiterdaten einlesen, wenn Anzeige-Button gedrückt wurde, Personen
// vorhanden sind und keine Fehler vorliegen
if($_POST['Button_Anzeigen'] && fxIsArray($pers_arr) && !$fehler && ($awerr < 2))
{
	$personen=array();
	$pszugeordnet=0;
	$von_min=$von;
	$bis_max=$bis;

	$zeile=2;  // 0: Monat, 1: KW, 2: Tag
	$icnt=0;

	// Bezeichnungen der Sondertage
	$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=434 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0";
	$uskname=get_array($sql, 'wertetabid', 'tabwert');

	// 1. Schritt: Alle zugeordneten Aufgaben der einzelnen Personen ermitteln
	$sql  = "SELECT";
	$sql .= " m.personen_id, m.projekt_id, m.auslastung, m.aufwand_std AS aufwand_sek, m.prozent, m.soll_beginn, m.soll_ende,";
	$sql .= " p.mutterprojekt_id, p.vorgangsnummer, p.name_projekt, p. prioritaet, p.projekt_status,";
	$sql .= " 0 AS ist_sek, 0 AS fgrad, 0 AS atyp, 1 AS disp, b.budget_pers_int_gepl, b.budget_pers_int_gen ";
	$sql .= "FROM";
	$sql .= " maplanung m, projekte p, budget_summe b ";
	$sql .= "WHERE";
	$sql .= " m.personen_id IN (".$pers_ids.") AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND p.projekt_id=m.projekt_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.projektart=".FXP_PRJ_TASK;
	$sql .= " AND b.projekt_id=m.projekt_id AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND ((p.projekt_status<>".FXP_PS_COMPLETED." AND p.projekt_status<>".FXP_PS_INACTIVE." AND (";
	if(strlen($iau))
		$sql .= "p.projekt_id IN (".$iau.") OR ";
	$sql .= "(m.soll_beginn<='".$bis."' AND m.soll_ende>='".$von."')";
	$sql .= "))";
	if(!$_POST['nur_plandaten'])
		$sql .= " OR (m.soll_beginn<='".$bis."' AND p.projekt_status=".FXP_PS_ACTIVE." AND m.aufwand_std>0)";
	$sql .= ") ";
	$sql .= "ORDER BY";
	$sql .= " m.personen_id, p.vorgangsnummer";
	$pers_prj=db_values($sql, '*');
//fxDebug($pers_prj, $sql);

	// 2. Schritt:	Personendaten der Mitarbeiter aussortieren
	$faeorder=array();
	foreach($pers_arr as $p)
	{
		$i=(int)$p['personen_id'];

		$hinzufg=false;
		if(strlen($fgr_ids) || strlen($fae_ids))
		{
			// prüfen, ob Person die erforderliche(n) Fähigkeit(en) hat
			$fsql="SELECT personen_id FROM ma_art_faehigkeiten WHERE personen_id=".$i." AND (";
			if(strlen($fgr_ids))
				$fsql .= "maart_id IN (".$fgr_ids.")";
			if(strlen($fae_ids))
			{
				if(strlen($fgr_ids))
					$fsql .= " OR ";
				$fsql .= "mafaehigkeit_id IN (".$fae_ids.")";
			}
			$fsql .= ") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$fdata=db_values($fsql, '*');
//fxDebug($fdata, $fsql);
			if(fxIsArray($fdata))
			{
				$hinzufg=true;

				foreach($fdata as $f)
				{
					if(!sizeof($faeorder))
						$faeorder=array($i => 1);
					else if(!isset($faeorder[$i]))
						$faeorder[$i]=1;
					else
						$faeorder[$i]++;
				}
			}
		}
		else
			$hinzufg=true;

		// 3. Schritt:	Personendaten der Mitarbeiter auslesen
		//				(Name, verfügbare Wochentage)
		$ma_zeile[$i]=++$zeile;
		$personen[$i]=array();
		$personen[$i]['id']=$i;
		$personen[$i]['fullname']=$p['pname'];
		$vn=trim($p['vorname']);
		if(strlen($vn))
			$personen[$i]['fullname'] .= ',&nbsp;'.$vn;
		$personen[$i]['aktiv']=$hinzufg;
		$personen[$i]['paktiv']=0;
		$personen[$i]['ptyp']=$p['personentyp'];
		$personen[$i]['sex']=$p['geschlecht'];
		$personen[$i]['tage']=0;
		for($wtc=0; $wtc<7; $wtc++)
		{
			$personen[$i][$wtc]=0;
			if(fxIsArray($madaten[$i]) && $madaten[$i][chr(0x61 + $wtc)])
				$personen[$i][$wtc]=1;
		}
		// Mandanten-ID hinzufügen
		$personen[$i]['mandanten_id']=$p['mandanten_id'];
		// Arbeitstage hinzufügen
		$personen[$i]['arbeitstage']=0;
		$personen[$i]['wochenarbeitstd']=0.0;
		$personen[$i]['startzeit']='';
		$personen[$i]['pause']=0;
		$personen[$i]['eintritt']='00000000000000';
		$personen[$i]['austritt']='';
		if(fxIsArray($madaten[$i]))
		{
			if($madaten[$i]['arbeitstage'])
				$personen[$i]['arbeitstage']=$madaten[$i]['arbeitstage'];
			if($madaten[$i]['wochenarbeitstd'])
				$personen[$i]['wochenarbeitstd']=$madaten[$i]['wochenarbeitstd'];
			$personen[$i]['startzeit']=$madaten[$i]['startzeit'];
			$personen[$i]['pause']=$madaten[$i]['pause'];
			if(strlen($madaten[$i]['maeintrittsdatum']))
				$personen[$i]['eintritt']=$madaten[$i]['maeintrittsdatum'];
			if(strlen($madaten[$i]['maaustrittsdatum']))
				$personen[$i]['austritt']=$madaten[$i]['maaustrittsdatum'];
		}
		$personen[$i]['aufwand_sek']=0.0;
		$personen[$i]['prozent']=0.0;
		$personen[$i]['d_aufw']=false;

		$personen[$i]['maus']['sstd']=0.0;
		$personen[$i]['maus']['gstd']=0.0;
		$personen[$i]['maus']['astd_ist']=0.0;
		$personen[$i]['maus']['astd_ges']=0.0;
		$personen[$i]['maus']['astd_term']=0.0;
		$personen[$i]['maus']['astd_akt']=0.0;
		$personen[$i]['maus']['astd_gen']=0.0;
		$personen[$i]['maus']['astd_gepl']=0.0;
		if($personen[$i]['arbeitstage'] > 0)
			$personen[$i]['maus']['sstd']=$personen[$i]['wochenarbeitstd']/$personen[$i]['arbeitstage'];
		// EK, VK hinzufügen, falls Projekt gewählt
		$ek_vk=array();
		if($assign_granted && $iau)
		{
			$durchschnitt_ek=false;
			$durchschnitt_vk=false;
			// Standard-EK + Standard-VK
			$ekvk_sql="SELECT ek, vk, ek AS s_ek, vk AS s_vk, ek AS d_ek, vk AS d_vk, artikel_id FROM artikel WHERE personen_id=".$i." AND (sachmittel_id=0 OR sachmittel_id IS NULL) AND (archiv=0 OR archiv IS NULL) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$personen[$i]['kosten']=db_values($ekvk_sql);
			if(!$personen[$i]['kosten']['ek'])
				$personen[$i]['kosten']['ek']=0.0;
			if(!$personen[$i]['kosten']['vk'])
				$personen[$i]['kosten']['vk']=0.0;
			if(!$personen[$i]['kosten']['s_ek'])
				$personen[$i]['kosten']['s_ek']=0.0;
			if(!$personen[$i]['kosten']['s_vk'])
				$personen[$i]['kosten']['s_vk']=0.0;
			if(!$personen[$i]['kosten']['d_ek'])
				$personen[$i]['kosten']['d_ek']=0.0;
			if(!$personen[$i]['kosten']['d_vk'])
				$personen[$i]['kosten']['d_vk']=0.0;
			if(!$personen[$i]['kosten']['artikel_id'])
				$personen[$i]['kosten']['artikel_id']=0;
			$personen[$i]['kosten']['typ']=0;
			// falls schon Person zu Aufgabe zugewiesen, dann EK + VK übernehmen
			$durchschnitt_ek=false;
			$durchschnitt_vk=false;
			$kosten_sql  = "SELECT p.projekt_id, a.ek_projekt, a.vk_projekt, z.soll_beginn, z.soll_ende";
			$kosten_sql .= " FROM artikelzuordnung a, projekte p, zeitdaten z";
			$kosten_sql .= " WHERE p.projekt_id=a.projekt_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.projektart=".FXP_PRJ_TASK;
			$kosten_sql .= " AND a.artikel_id=".$personen[$i]['kosten']['artikel_id']." AND a.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND a.projekt_id IN (".$iau.")";
			$kosten_sql .= " AND z.projekt_id=p.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$kosten=db_values($kosten_sql, '*');
			if(fxIsArray($kosten))
			{
//fxDebug($kosten);
				$mstd=0.0;
				if($personen[$i]['arbeitstage'] > 0)
					$mstd=(double)($personen[$i]['wochenarbeitstd']/$personen[$i]['arbeitstage']);
				if(!$mstd)
					$mstd=(double)$GLOBALS['fxpglobals']['settings']['mtag'];
				$ek_d=0.0;
				$vk_d=0.0;
				$ek_v='';
				$vk_v='';
				$evz=0.0;
				foreach($kosten as $kst)
				{
					$td=((double)fxp_date_diff($kst['soll_beginn'], $kst['soll_ende'])+1.00)*$mstd;
					$ek_d += $kst['ek_projekt']*$td;
					$vk_d += $kst['vk_projekt']*$td;
					$evz += $td;
					$ek_vk[$kst['projekt_id']]=array('ek'=>$kst['ek_projekt'], 'vk'=>$kst['vk_projekt']);
					if(!$ek_v)
						$ek_v=$kst['ek_projekt'];
					else if($ek_v != $kst['ek_projekt'])
						$durchschnitt_ek=true;
					if(!$vk_v)
						$vk_v=$kst['vk_projekt'];
					else if($vk_v != $kst['vk_projekt'])
						$durchschnitt_vk=true;
				}
//echo('$ek_d='.$ek_d.', $vk_d='.$vk_d.'<br>');
				$personen[$i]['kosten']['ek']=(double)($ek_d/$evz);
				$personen[$i]['kosten']['vk']=(double)($vk_d/$evz);
				$personen[$i]['kosten']['d_ek']=(double)($ek_d/$evz);
				$personen[$i]['kosten']['d_vk']=(double)($vk_d/$evz);
				$personen[$i]['kosten']['typ']=2;
//echo('ek='.$personen[$i]['kosten']['ek'].', vk='.$personen[$i]['kosten']['vk'].'<br>');
			}
			if($durchschnitt_ek)
				$global_durchschnitt_ek=true;
			if($durchschnitt_vk)
				$global_durchschnitt_vk=true;
		}
		// Projekte hinzufügen
		if(fxIsArray($pers_prj))
		{
			$pz_v='';
			foreach($pers_prj as $pj)
			{
				if($pj['personen_id'] == $i)
				{
					$pjc=(int)$pj['projekt_id'];

					// Kalkulationsenddatum evtl. anpassen
					$pj['ber_ende']=$pj['soll_ende'];
					if(!$_POST['nur_plandaten'] && ($pj['projekt_status'] == FXP_PS_ACTIVE) && ($pj['ber_ende'] < $today))
						$pj['ber_ende']=substr($today, 0, 8).'240000';

					// Aufgabentyp festlegen: 0=aktiv (Standard), 1=geplant ohne Budgetgenehmigung, 2=geplant mit Budgetgenehmigung
					if($pj['projekt_status'] != FXP_PS_ACTIVE)
					{
						$pj['atyp']=1;
						if($pj['budget_pers_int_gen'])
							$pj['atyp']=2;
					}
//echo($i.': projekt_id='.$pj['projekt_id'].', status='.$pj['projekt_status'].', soll_beginn='.$pj['soll_beginn'].', soll_ende='.$pj['soll_ende'].', auslastung='.$pj['auslastung'].', atyp='.$pj['atyp'].', budget_genehmigt='.$pj['budget_pers_int_gen'].'<br>'.$nl);

					if(!sizeof($pers_zug) || !$pers_zug[$i][$pj['projekt_id']])
					{
						// Wartungsaufgaben überspringen?
						if(!$pj['aufwand_sek'] && !$_POST['wartung_anz'])
						{
//echo(' -&gt; Wartungsaufgabe irrelevant, da kein Ausfwand: <b>SKIP!</b><br>');
							continue;
						}
						// Aktive Aufgaben anzeigen?
						else if(!$pj['atyp'] && !$_POST['atyp_akt'])
						{
//echo(' -&gt; Aktive Aufgabe nicht anzeigen: <b>SKIP!</b><br>');
							$pj['disp']=0;
						}
						// Genehmigte Aufgaben anzeigen?
						else if(($pj['atyp'] == 2) && !$_POST['atyp_gen'])
						{
//echo(' -&gt; Genehmigte Aufgabe nicht anzeigen: <b>SKIP!</b><br>');
							$pj['disp']=0;
						}
						// Geplante Aufgaben anzeigen?
						else if(($pj['atyp'] == 1) && !$_POST['atyp_gepl'])
						{
//echo(' -&gt; Geplante Aufgabe nicht anzeigen: <b>SKIP!</b><br>');
							$pj['disp']=0;
						}
					}

					$personen[$i]['projekte'][$pjc]=$pj;

					if($pj['disp'])
					{
						$von_min=min($pj['soll_beginn'], $von_min);
						$bis_max=max($pj['ber_ende'], $bis_max);
					}

					// Mitarbeiter zu Aufgabe zugeordnet
					$aufg_zu=false;
					if(sizeof($pers_zug) && $pers_zug[$i][$pj['projekt_id']])
					{
						$personen[$i]['aufwand_sek'] += (double)$pj['aufwand_sek'];
						$personen[$i]['prozent'] += (double)$pj['prozent'];
						if(!$pz_v)
							$pz_v=$pj['prozent'];
						else if($pz_v != $pj['prozent'])
						{
							$personen[$i]['d_aufw']=true;
							$global_durchschnitt_aufw=true;
						}
//echo($i.': $pz_v='.$pz_v.'<br>');
						if(fxIsArray($ek_vk[$pj['projekt_id']]))
						{
							$personen[$i]['projekte'][$pjc]['ek']=(double)$ek_vk[$pj['projekt_id']]['ek'];
							$personen[$i]['projekte'][$pjc]['vk']=(double)$ek_vk[$pj['projekt_id']]['vk'];
						}
						$aufg_zu=true;
						$personen[$i]['paktiv']++;
					}
					$personen[$i]['projekte'][$pjc]['zugeordnet']=$aufg_zu;
				}
			}
//echo('<hr>'.$nl);
			if($personen[$i]['paktiv'])
				$personen[$i]['prozent'] /= (double)$personen[$i]['paktiv'];
		}
		if($personen[$i]['paktiv'])
			$pszugeordnet++;

		$icnt++;
	}
fxDebug($personen,'$personen', 3);

	// Daten für Anzeige ermitteln nur, wenn Personen gefunden
	if(fxIsArray($personen))
	{
		// Nach Fähigkeiten sortieren, falls ausgewählt
		if(sizeof($faeorder))
		{
//fxDebug($faeorder, 'faeorder');
			$faeneu=array();
			while(sizeof($faeorder))
			{
				$npid=0;
				$oanz=0;
				foreach($faeorder as $fpid => $fanz)
				{
					if($fanz > $oanz)
					{
						$npid=$fpid;
						$oanz=$fanz;
					}
				}
				$faeneu[$npid]=$oanz;
				unset($faeorder[$npid]);
			}
			if(sizeof($faeneu))
			{
//fxDebug($faeneu, 'faeneu');
				$personen_backup=$personen;
				unset($personen);
				$personen=false;
				foreach($faeneu as $persid => $dmy)
				{
					$personen[$persid]=$personen_backup[$persid];
					unset($personen_backup[$persid]);
				}
				if(fxIsArray($personen_backup))
				{
					foreach($personen_backup as $persid => $persdat)
						$personen[$persid]=$personen_backup[$persid];
					unset($personen_backup);
				}
			}
		}

		// Kalender erstellen
		if($assign_granted && $iau)
		{
			if($con || $GLOBALS['fxpglobals']['persdat']['roles'][-3] || $GLOBALS['fxpglobals']['persdat']['roles'][1] || $GLOBALS['fxpglobals']['persdat']['roles'][2])
			{
				if($praufg)
					$xoffset=5;
				else
					$xoffset=6;

				if($budgetart == 'N')
					$xoffset -= 2;
				else if(($budgetart == 'I') || ($budgetart == 'E'))
					$xoffset--;
			}
			else
				$xoffset=2;
		}
		else
			$xoffset=1;

		// Auslastung anzeigen?
		if($_POST['ZAuslastung'])
			$xoffset++;

		// Zeile 0 (0+1): Projekt + Aufwand oder Fähigkeiten
		$pr_tooltip='';
		if($assign_granted && $iau)
		{
			$bpig=get_text(1754, '20');
			$bpeg=get_text(1755, '20');
			$ekg =get_text(1243, 'text');	// kalkulierter EK (h)
			$vkg =get_text(1780, 'text');	// kalkulierter VK (h)
			$ekgv='';
			$vkgv='';

			$prsql  = "SELECT p.vorgangsnummer, p.name_projekt, p.kunde, p.projektleiter, p.ansprechpartner, p.projekt_status, p.prioritaet, p.aufwand_soll, p.aufwand_ist, p.deadline, p.max_zeit_aufw, p.ref_nummer, p.maart_id, p.mafaehigkeit_id,";
			$prsql .= " b.budget_pers_int_gepl, b.budget_pers_ext_gepl,";
			$prsql .= " z.soll_beginn, z.soll_ende";
			$prsql .= " FROM projekte p, budget_summe b, zeitdaten z";
			$prsql .= " WHERE b.projekt_id=p.projekt_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$prsql .= " AND z.projekt_id=p.projekt_id AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.projekt_id IN (".$iau.") AND z.zeitart=102";
			$prsql .= " ORDER BY p.vorgangsnummer";
			$prarr=db_values($prsql, '*');
//fxDebug($prarr, $prsql);

			if(fxIsArray($prarr) && (sizeof($prarr) == 1))
			{
				$prarr=$prarr[0];
//fxDebug($prarr);
				// TOOLTIP: Name
				$pr_nm='<b>'.no_null($prarr['vorgangsnummer']).'</b>&nbsp;'.strtr($prarr['name_projekt'], $GLOBALS['caption_strtr']);
				$pr_tooltip_js	= '<table border=0 cellpadding=0 cellspacing=1>';

				// Zeitspanne
				$pr_tooltip_js	.= '<tr><td nowrap><b>'.$pb_string['zeitspanne'].':</b></td><td nowrap>&nbsp;'.get_entry($prarr['soll_beginn'], 'datum').'-'.get_entry($prarr['soll_ende'], 'datum').'</td></tr>';
				// Ist-/Soll-Aufwand
				$pr_tooltip_js	.= '<tr><td nowrap><b>'.$pb_string['isa'].':</b></td><td nowrap>&nbsp;'.get_entry($prarr['aufwand_ist']/3600, 'dezimal').'&nbsp;h&nbsp;/&nbsp;'.get_entry($prarr['aufwand_soll']/3600, 'dezimal').'&nbsp;h</td></tr>';
				// max.Zeitaufwand
				if($prarr['max_zeit_aufw'])
					$pr_tooltip_js	.= '<tr><td nowrap><b>'.$pb_string['mz'].':</b></td><td nowrap>&nbsp;'.get_entry($prarr['max_zeit_aufw']/3600, 'dezimal').'&nbsp;h</td></tr>';
				// Deadline
				if($prarr['deadline'])
					$pr_tooltip_js	.= '<tr><td nowrap><b>'.$pb_string['dead'].':</b></td><td nowrap>&nbsp;'.get_entry($prarr['deadline'], 'datum').'</td></tr>';

				// Personenbudget gepl. (int/ext)
				if($prarr['aufwand_soll'])
				{
					$ekgv=round($prarr['budget_pers_int_gepl']*3600/$prarr['aufwand_soll'], 2);
					$vkgv=round($prarr['budget_pers_ext_gepl']*3600/$prarr['aufwand_soll'], 2);
				}
				else
				{
					$ekgv=0.0;
					$vkgv=0.0;
				}
				if($budgetart != 'N')
				{
					$pr_tooltip_js	.= '<tr><td nowrap colspan=2><br><hr class=fxhr><br></td></tr>';
					$ttbr='';
					if($budgetart == 'B')
						$ttbr='<br><br>';
					if(($budgetart == 'I') || ($budgetart == 'B'))
					{
						$pr_tooltip_js	.= '<tr><td nowrap><b>'.$bpig.':</b></td><td nowrap align=right>'.get_entry($prarr['budget_pers_int_gepl'], 'dezimal').'&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'].'&nbsp;</td></tr>';
						$pr_tooltip_js	.= '<tr><td nowrap Valign=top>-&gt;&nbsp;<b>'.$ekg.':</b></td><td nowrap align=right>'.get_entry($ekgv, 'dezimal').'&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'].'&nbsp;'.$ttbr.'</td></tr>';
					}
					if(($budgetart == 'E') || ($budgetart == 'B'))
					{
						$pr_tooltip_js	.= '<tr><td nowrap><b>'.$bpeg.':</b></td><td nowrap align=right>'.get_entry($prarr['budget_pers_ext_gepl'], 'dezimal').'&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'].'&nbsp;</td></tr>';
						$pr_tooltip_js	.= '<tr><td nowrap>-&gt;&nbsp;<b>'.$vkg.':</b></td><td nowrap align=right>'.get_entry($vkgv, 'dezimal').'&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'].'&nbsp;</td></tr>';
					}
				}

				// Projekt-Status
				$prs=meldung($prarr['projekt_status']);
				$pr_tooltip_js	.= '<tr><td nowrap colspan=2><br><hr class=fxhr><br></td></tr>';
				$pr_tooltip_js	.= '<tr><td nowrap><b>'.$pb_string['prs'].':</b></td><td nowrap>&nbsp;'.$prs.'</td></tr>';
				// Priorität
				$prp=meldung($prarr['prioritaet']);
				$pr_tooltip_js	.= '<tr><td nowrap><b>'.$pb_string['prio'].':</b></td><td nowrap>&nbsp;'.$prp.'</td></tr>';
				// Faehigkeit
				if($prarr['maart_id'])
				{
					$prfg=db_value("SELECT maart_name FROM ma_art WHERE maart_id=".$prarr['maart_id']." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")");
					if(strlen($prfg))
					{
						$pr_tooltip_js	.= '<tr><td nowrap><b>'.$pb_string['fae'].':</b></td><td nowrap>&nbsp;'.str_replace("'", "´", $prfg);
						if($prarr['mafaehigkeit_id'])
						{
							$prfg=db_value("SELECT mafaehigkeit_name FROM ma_faehigkeit WHERE mafaehigkeit_id=".$prarr['mafaehigkeit_id']." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")");
							if(strlen($prfg))
								$pr_tooltip_js	.= '&nbsp;->&nbsp;'.str_replace("'", "´", $prfg);
							else
								$pr_tooltip_js	.= '*';
						}
						$pr_tooltip_js	.= '</td></tr>';
					}
				}

				// Projektleiter
				$pr_tooltip_js	.= '<tr><td nowrap colspan=2><br><hr class=fxhr><br></td></tr>';
				$pr_tooltip_js	.= '<tr><td nowrap><b>'.$pb_string['pl'].':</b></td><td nowrap>&nbsp;'.str_replace('"', '´', person_pic($prarr['projektleiter'])).'</td></tr>';
				// Ansprechpartner
				if($prarr['ansprechpartner'])
					$pr_tooltip_js	.= '<tr><td nowrap><b>'.$pb_string['ansp'].':</b></td><td nowrap>&nbsp;'.str_replace('"', '´', person_pic($prarr['ansprechpartner'])).'</td></tr>';
				// Auftraggeber
				if($prarr['kunde'])
					$pr_tooltip_js	.= '<tr><td nowrap><b>'.$pb_string['kunde'].':</b></td><td nowrap>&nbsp;'.str_replace('"', '´', person_pic($prarr['kunde'])).'</td></tr>';
				$pr_tooltip_js	.= '</table>';
				$pr_tooltip_js='<table width=100%><tr><td width=100% bgcolor=#dddddd>&nbsp;<b class=s4>'.str_replace("'", "´", $pr_nm).'</b>&nbsp;</td></tr></table><br>'.$pr_tooltip_js;

				$pr_gesamtaufwand=$prarr['aufwand_soll'];
				$GLOBALS['fxptdata']['cal'][0][0]['text']='&nbsp;['.no_null($prarr['vorgangsnummer']).']&nbsp;'.strtr($prarr['name_projekt'], $GLOBALS['caption_strtr']).'&nbsp;<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top">&nbsp;&nbsp;<font color="#cc0000">('.get_entry($pr_gesamtaufwand/3600, 'dezimal').'&nbsp;h)</font>';
			}
			else if(fxIsArray($prarr))
			{
				$aufgmld=meldung(1475, true);	// Aufgaben

				$ttac=0;
				$pr_gesamtaufwand=0.0;
				$pr_istaufwand=0.0;
				$pr_bpi=0.0;
				$pr_bpe=0.0;

				$pr_tooltip_js='<table border=0 cellpadding=0 cellspacing=1>';
				foreach($prarr as $p)
				{
					if($ttac == 30)
						$pr_tooltip_js	.= '<tr><td colspan=2>...</td></tr>';
					else if($ttac < 30)
					{
						$vn=no_null($p['vorgangsnummer']);
						$va=get_entry($p['aufwand_soll']/3600, 'dezimal').'&nbsp;h';
						$pr_tooltip_js	.= '<tr><td nowrap>&nbsp;<b class=black>'.$vn.'</b>&nbsp;-&nbsp;'.strtr($p['name_projekt'], $GLOBALS['caption_strtr']).'&nbsp;</td><td align=right>&nbsp;&nbsp;'.$va.'&nbsp;</td></tr>';
					}
					$pr_gesamtaufwand += $p['aufwand_soll'];
					$pr_istaufwand += $p['aufwand_ist'];
					$pr_bpi += $p['budget_pers_int_gepl'];
					$pr_bpe += $p['budget_pers_ext_gepl'];
					$ttac++;
				}
				// Ist-/Soll-Aufwand
				$pr_tooltip_js	.= '<tr><td nowrap colspan=2><br><hr class=fxhr><br></td></tr>';
				$pr_tooltip_js	.= '<tr><td nowrap><b class=black>'.$pb_string['isa'].':</b></td><td nowrap>&nbsp;'.get_entry($pr_istaufwand/3600, 'dezimal').'&nbsp;h&nbsp;/&nbsp;'.get_entry($pr_gesamtaufwand/3600, 'dezimal').'&nbsp;h</td></tr>';
				// Personenbudget gepl. (int/ext)
				if($pr_gesamtaufwand)
				{
					$ekgv=round($pr_bpi*3600/$pr_gesamtaufwand, 2);
					$vkgv=round($pr_bpe*3600/$pr_gesamtaufwand, 2);
				}
				else
				{
					$ekgv=0.0;
					$vkgv=0.0;
				}
				$pr_tooltip_js	.= '<tr><td nowrap colspan=2><br><hr class=fxhr><br></td></tr>';
				$pr_tooltip_js	.= '<tr><td nowrap><b class=black>'.$bpig.':</b></td><td nowrap align=right>'.get_entry($pr_bpi, 'dezimal').'&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'].'&nbsp;</td></tr>';
				$pr_tooltip_js	.= '<tr><td nowrap valign=top>-&gt;&nbsp;<b class=blue>'.$ekg.':</b></td><td nowrap align=right>'.get_entry($ekgv, 'dezimal').'&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'].'&nbsp;<br><br></td></tr>';
				$pr_tooltip_js	.= '<tr><td nowrap><b class=black>'.$bpeg.':</b></td><td nowrap align=right>'.get_entry($pr_bpe, 'dezimal').'&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'].'&nbsp;</td></tr>';
				$pr_tooltip_js	.= '<tr><td nowrap valign=top>-&gt;&nbsp;<b class=blue>'.$vkg.':</b></td><td nowrap align=right>'.get_entry($vkgv, 'dezimal').'&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'].'&nbsp;</td></tr>';

				$pr_tooltip_js	.= '</table>';
				$pr_tooltip_js='<table width=100%><tr><td width=100% bgcolor=#dddddd>&nbsp;<b class=s4>'.str_replace("'", "´", $aufgmld).'</b>&nbsp;</td></tr></table><br>'.$pr_tooltip_js;

				// Aufgaben
				$GLOBALS['fxptdata']['cal'][0][0]['text']='&nbsp;<b>'.$aufgmld.'</b>&nbsp;<font class="grey">('.get_entry($pr_gesamtaufwand/3600, 'dezimal').'&nbsp;h)</font>';
			}
			else
				$GLOBALS['fxptdata']['cal'][0][0]['text']='&nbsp;<i class="red">???</i>';
		}
		else if(fxIsArray($MaFaehigkeit_ID) && $MaFaehigkeit_ID[0])
		{
			$ftxt='';
			foreach($MaFaehigkeit_ID as $fid)
			{
				if(strlen($ftxt))
					$ftxt .= ',&nbsp;';
				if(substr($fid, -1) == 'f')
					$ftxt .= db_value("SELECT mafaehigkeit_name FROM ma_faehigkeit WHERE mafaehigkeit_id=".(int)$fid." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")");
				else
					$ftxt .= '['.db_value("SELECT maart_name FROM ma_art WHERE maart_id=".(int)$fid." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")").']';
			}
			$GLOBALS['fxptdata']['cal'][0][0]['text']='&nbsp;'.$ftxt;
		}
		else
			$GLOBALS['fxptdata']['cal'][0][0]['text']='&nbsp;'.meldung(1130);	// Alle Fähigkeiten
		$GLOBALS['fxptdata']['cal'][0][0]['color']='ffffff';
		if($pr_tooltip_js)
			$GLOBALS['fxptdata']['cal'][0][0]['tooltip_js']=$pr_tooltip_js;
		$GLOBALS['fxptdata']['cal'][0][0]['colspan']=$xoffset;
		$GLOBALS['fxptdata']['cal'][0][0]['typ']='All';

		// Feiertage im angegebenen Zeitraum ermitteln
		if($_POST['ZAuslastung'] || $_POST['ZProjekte'])
			$holidays=get_feiertage($von_min, $bis_max, $GLOBALS['fxpglobals']['client']);
		else
			$holidays=array();

		// Zeile 1 (0+1): Zeitspanne
		$psvon=formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $von);
		$psbis=formatdatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $bis);
		$zs=$psvon.'&nbsp;-&nbsp;'.$psbis; // Von - Bis
		$GLOBALS['fxptdata']['cal'][1][0]['text']='&nbsp;<b>'.$pb_string['zeitspanne'].'</b>:&nbsp;'.$zs.'&nbsp;';
		$GLOBALS['fxptdata']['cal'][1][0]['color']='ffffff';
		$GLOBALS['fxptdata']['cal'][1][0]['colspan']=$xoffset;
		$GLOBALS['fxptdata']['cal'][1][0]['typ']='All';

		// Zeile 2 (0-$xoffset): Überschriften "Mitarbeiter", ..., "Info"
		$cc=0;
		if($assign_granted && $iau && ($con || $GLOBALS['fxpglobals']['persdat']['roles'][-3] || $GLOBALS['fxpglobals']['persdat']['roles'][1] || $GLOBALS['fxpglobals']['persdat']['roles'][2]))
		{
			// Zeile 2: OK=Zuordnen
			$GLOBALS['fxptdata']['cal'][2][$cc]['text']='<font class="s4b">'.$pb_string['ok'].'</font><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top">'; // OK.
			$GLOBALS['fxptdata']['cal'][2][$cc]['color']='dddddd';
			$GLOBALS['fxptdata']['cal'][2][$cc]['align']='center';
			$GLOBALS['fxptdata']['cal'][2][$cc]['typ']='All';
			$GLOBALS['fxptdata']['cal'][2][$cc]['tooltip_js']=meldung(1713);	// Mitarbeiter dem Projekt/Teilprojekt oder der Aufgabe zuordnen (Haken überwiegt Aufwandsangaben).
			$cc++;
		}

		$GLOBALS['fxptdata']['cal'][2][$cc]['text']='&nbsp;<b>'.$pb_string['mitarbeiter'].'</b>&nbsp;'; // Mitarbeiter
		$GLOBALS['fxptdata']['cal'][2][$cc]['color']='dddddd';
		$GLOBALS['fxptdata']['cal'][2][$cc]['typ']='All';

		if($assign_granted && $iau)
		{
			if(!$praufg)
			{
				// Zeile 2, Spalte 1: Projekt
				$cc++;
				$GLOBALS['fxptdata']['cal'][2][$cc]['text']='&nbsp;<b>'.$pb_string['pj'].'</b>&nbsp;'; // Prj.
				$GLOBALS['fxptdata']['cal'][2][$cc]['color']='dddddd';
				$GLOBALS['fxptdata']['cal'][2][$cc]['align']='center';
				$GLOBALS['fxptdata']['cal'][2][$cc]['typ']='All';
			}
			if($con || $GLOBALS['fxpglobals']['persdat']['roles'][-3] || $GLOBALS['fxpglobals']['persdat']['roles'][1] || $GLOBALS['fxpglobals']['persdat']['roles'][2])
			{
				// Zeile 2, Spalte 2: EK
				if(($budgetart == 'I') || ($budgetart == 'B'))
				{
					$cc++;
					$slit='<b>'.$pb_string['ek'].'</b>';
					if($ekgv > 0.0)
						$slit .= '<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top">';
					$GLOBALS['fxptdata']['cal'][2][$cc]['text']=$slit; // EK
					$GLOBALS['fxptdata']['cal'][2][$cc]['color']='dddddd';
					$GLOBALS['fxptdata']['cal'][2][$cc]['width']='74';
					$GLOBALS['fxptdata']['cal'][2][$cc]['align']='center';
					$GLOBALS['fxptdata']['cal'][2][$cc]['typ']='All';
					if($ekgv > 0.0)
						$GLOBALS['fxptdata']['cal'][2][$cc]['tooltip_js']=$ekg.': '.get_entry($ekgv, 'dezimal').'&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'];
				}

				// Zeile 2, Spalte 3: VK
				if(($budgetart == 'E') || ($budgetart == 'B'))
				{
					$cc++;
					$slit='<b>'.$pb_string['vk'].'</b>';
					if($vkgv > 0.0)
						$slit .= '<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top">';
					$GLOBALS['fxptdata']['cal'][2][$cc]['text']=$slit; // VK
					$GLOBALS['fxptdata']['cal'][2][$cc]['color']='dddddd';
					$GLOBALS['fxptdata']['cal'][2][$cc]['width']='74';
					$GLOBALS['fxptdata']['cal'][2][$cc]['align']='center';
					$GLOBALS['fxptdata']['cal'][2][$cc]['typ']='All';
					if($vkgv > 0.0)
						$GLOBALS['fxptdata']['cal'][2][$cc]['tooltip_js']=$vkg.': '.get_entry($vkgv, 'dezimal').'&nbsp;'.$GLOBALS['fxpglobals']['settings']['waehrung'];
				}

				// Zeile 2, Spalte 4: Aufwand
				$cc++;
				$GLOBALS['fxptdata']['cal'][2][$cc]['text']='<b>'.$pb_string['aufw'].'</b>'; // Aufw.
				$GLOBALS['fxptdata']['cal'][2][$cc]['color']='dddddd';
				$GLOBALS['fxptdata']['cal'][2][$cc]['align']='center';
				$GLOBALS['fxptdata']['cal'][2][$cc]['typ']='All';
			}
		}

		if($_POST['ZAuslastung'])
		{
			// Zeile 2, Spalte 1 (7): Auslastung
			$cc++;
			$GLOBALS['fxptdata']['cal'][2][$cc]['text']='<b>'.$pb_string['ausl'].'</b>'; // Auslastung
			$GLOBALS['fxptdata']['cal'][2][$cc]['color']='dddddd';
			$GLOBALS['fxptdata']['cal'][2][$cc]['align']='center';
			$GLOBALS['fxptdata']['cal'][2][$cc]['typ']='All';
		}

		// Zeile 0-2: Loop über alle Spalten des Zeitraums
		if($_POST['ZAuslastung'] || $_POST['ZProjekte'])
			$xcolumns=kalendertage($von, $bis, $ke);
		else
			$xcolumns=0;

		// Zeile 3-?: Personen + Projekte eintragen in jede Zeile
		$aktzeile=3;
		$perszeile=0;
		foreach($personen as $persid => $pers)
		{
			// Alle Mitarbeiter anzeigen, die entweder die passende Fähigkeit haben oder bereits einem Projekt zugeordnet sind
			if(($con || $GLOBALS['fxpglobals']['persdat']['roles'][-3] || $GLOBALS['fxpglobals']['persdat']['roles'][1] || $GLOBALS['fxpglobals']['persdat']['roles'][2]) && ($pers['aktiv'] || $pers['paktiv']))
			{
				kalenderzeile($persid, $aktzeile, $GLOBALS['__DGBLVL'], $pb_string, $ke);
				$perszeile++;
			}
		}

		// Zusatzzeile: Durchschnittsauslastung aller angezeigten Mitarbeiter (wird in ctools.inc, Funktion kalenderzeile() gesetzt und errechnet)
		if(($aktzeile > 3) && ($perszeile > 1) && $xcolumns && fxIsArray($delta_ausl_array))
		{
//fxDebug($GLOBALS['delta_ausl_array'], 'delta_ausl_array');
			// Leerzeile
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text']='~';
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['color']='dddddd';
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['height']=1;
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['colspan']=$xoffset;
			$GLOBALS['fxptdata']['cal'][$aktzeile][$xoffset]['text']='~';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$xoffset]['class']='ekr0';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$xoffset]['colspan']=$xcolumns-$xoffset+1;
			$aktzeile++;

			// Auslastungszeile
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['text']='<b>'.meldung(2855, true).':</b>&nbsp;';	//Durchschnittsauslastung aller angezeigter Personen
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['align']='right';
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['color']='ffffff';
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['colspan']=$xoffset-1;
			$GLOBALS['fxptdata']['cal'][$aktzeile][0]['typ']='All';
			$GLOBALS['fxptdata']['cal'][$aktzeile][$xoffset-1]=$delta_maus['cal'];
			verfuegbar($xoffset, $xcolumns-$xoffset+1, $aktzeile, $delta_ausl_array, array(), 'X');
			$aktzeile++;
		}

		// MA Zuordnen - Button
		if(($perszeile > 0) && $assign_granted && $iau && ($con || $GLOBALS['fxpglobals']['persdat']['roles'][-3] || $GLOBALS['fxpglobals']['persdat']['roles'][1] || $GLOBALS['fxpglobals']['persdat']['roles'][2]))
		{
			$btn=fieldSubmit('MAZ', meldung(1506,false));	// Mitarbeiter zuordnen
			$GLOBALS['fxpvars']['bbuffer'][-181]=$btn;
		}
	}	// end if: Daten für Anzeige ermitteln

	// EK/VK-Zuordnungs-Popup
	if($budgetart != 'N')
	{
		$zindex	= 512;

		$tdcs=4;
		if(($budgetart == 'I') || ($budgetart == 'E'))
			$tdcs--;
	}
}
//fxDebug($GLOBALS['fxptdata']['cal'],'cal');
?>