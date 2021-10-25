<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : urlaub_vj_pre.inc                                            //
// Version     : 21.2                                                         //
// Begin       : 2020-10-21                                                   //
// Last Change : 2020-10-21                                                   //
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
 * PF 142: Employee's Annual Vacation
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$index=true;

// Alle internen Mitarbeiter ermitteln mit eingestellten Auswahlkriterien
//	$GLOBALS['ignore_personentyp']=true;
$sql  = "SELECT p.personen_id, p.pname, p.vorname, p.geschlecht, p.personentyp, p.inaktiv, m.maeintrittsdatum, m.maaustrittsdatum, m.wochenarbeitstd, m.arbeitstage FROM personen p, madaten m";
$sql .= " WHERE p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.personentyp=".FXP_PT_EMPLOYEE." AND m.personen_id=p.personen_id AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
// ...Auswahlkriterium "Suchtext"
if(strlen($suchtext))
	$sql .= " AND (".db_like('p.pname','%'.$suchtext.'%')." OR ".db_like('p.vorname','%'.$suchtext.'%').")";
// ...Auswahlkriterium "Abteilung"
if($abt_id > 0)
{
	$acsv=getDepartments($GLOBALS['fxpglobals']['client'], $abt_id);
	$sql .= " AND p.abt_id IN (".$acsv.")";
}
// ...Auswahlkriterien "Zeitspanne von/bis"
if(strlen($zeitspanne_von) && strlen($zeitspanne_bis))
	$sql .= " AND m.maeintrittsdatum<='".$zeitspanne_bis."240000' AND (m.maaustrittsdatum IS NULL OR m.maaustrittsdatum>='".$zeitspanne_von."000000')";
else if(strlen($zeitspanne_von))
	$sql .= " AND m.maeintrittsdatum>='".$zeitspanne_von."000000'";
else if(strlen($zeitspanne_bis))
	$sql .= " AND (m.maaustrittsdatum IS NOT NULL AND m.maaustrittsdatum<='".$zeitspanne_bis."240000')";
// ...Auswahlkriterium "Aktive Mitarbeiter"
if($akt_ma)
	$sql .= " AND (p.inaktiv IS NULL OR p.inaktiv=0) AND (m.maaustrittsdatum IS NULL OR m.maaustrittsdatum>='".$GLOBALS['date']."000000')";
//	$sql .= filter_person();
$sql .= " ORDER BY p.pname, p.vorname";
$personen=db_values($sql, '*');
fxDebug($personen,$sql, 2);

// Personen gefunden?
if(fxIsArray($personen))
{
	$pids=array_to_csv($personen,'personen_id');

	// Urlaubsberechnung reseten?
	@reset_urlaubsberechnung();

	// NoWrap-Felder
	$Jahr_nw=true;
	$Urlaubsanspruch_nw=true;
	$Urlaub_vj_nw=true;
	$Status_Aktiv_nw=true;

	// Aktuelles Jahr
	$aj=(int)substr($GLOBALS['date'],0,4);

	// Werte im Klartext
	$va=explode('|', meldung(1887,false));	// Gesetzter Werte|Errechneter Wert|Keine gesetzten Werte vorhanden
	$ina='&nbsp;&nbsp;{'.meldung(1594, false).'}';	// inaktiv

	// Feldbreiten
	$wf=64;
	$ws=$wf+24;
	$wt=$ws*2;

	// Zeilenhöhe
	$Jahr_height=24;

	// Maskenwerte
	$mc=$GLOBALS['_maskcounter'];
	$mn=125;
	$ln=0;

	// Liste durchgehen
	foreach($personen as $pers)
	{
		$pid=(int)$pers['personen_id'];

		// resturlaub
		$ru=get_resturlaub($pid, '', '', false);
fxDebug($ru, $pid, 3);
		if(fxIsArray($ru))
		{
			// Leerzeile mit Top Border?
			if($ln)
			{
				fxf_maskColumnBorder(array('Jahr','Urlaubsanspruch','Urlaub_vj','Urlaub_gn','Status_Aktiv'), 1, -1, $mc, $ln, true);
				$ln++;
			}

			// Name über ganze Zeile
			$pname=trim($pers['pname']);
			$vname=trim($pers['vorname']);
			if(strlen($vname))
				$name=$pname.', '.$vname;
			else
				$name=$pname;

			$datum=get_entry($pers['maeintrittsdatum'], 'datum');
			$edatum='';
			if($pers['maaustrittsdatum'])
			{
				$edatum=substr($pers['maaustrittsdatum'], 0, 8);
				$datum .= '&nbsp;-&nbsp;'.get_entry($pers['maaustrittsdatum'], 'datum');
			}

			$dclass='black';
			if(strlen($edatum))
			{
				if($edatum < $GLOBALS['date'])
					$dclass='red';
				else
					$dclass='blue';
			}

			$iclass='';
			$itxt='';
			if($pers['inaktiv'])
			{
				$iclass=' class="red"';
				$itxt=$ina;
			}

			$txt='&nbsp;'.person_pic($pers['personen_id'], 20, $pers['geschlecht'], $GLOBALS['fxpglobals']['client'], false, 'border-top-left-radius:6px;').'&nbsp;&nbsp;<b'.$iclass.'>'.$name.'</b>&nbsp;<i class="'.$dclass.'">['.$datum.']</i>'.$itxt.'&nbsp;';
			fxf_listSectHeader($mc, $ln, $txt, 'jahr', 'status_aktiv', 5, 0, 1);
			$ln++;

			$dlc=0;
			foreach($ru as $uj => $ud)
			{
				// Zeile nur anzeigen wenn entweder Eintrag fix gesetzt oder Historienwert angezeigt werden sollen
				if(($ud['archiv'] != 0) && !$historie)
					continue;

				$dlc++;

				// Eintrag fix (0) oder errechnet (-1, 1)
				if($ud['archiv'] == 0)
					$cls='';
				else
					$cls=' class="grey"';

				// Nur fixe Einträge können gelöscht werden
				$dcb='';
				if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && ($ud['archiv'] == 0))
				{
					$chk=false;
					if($err && $_POST['Jahr'][$mc][$ln])
						$chk=true;
					$dcb=fieldCheckbox('Jahr['.$mc.']['.$ln.']', 1, $chk, false, false, false, '', '', '', false, 'position:absolute;left:0;top:-2px;');
				}

				// Jahr (Jahr)
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['jahr']='^class_?" height="24|~<div style="position:relative;">&nbsp;'.$dcb.'<span'.$cls.'>'.$uj.'</span>&nbsp;</div>';

				// Urlaub pro Jahr (Urlaubsanspruch: ua)
				$ua=(double)$ud['urlaub_pro_jahr'];
				if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
					$txt='~&nbsp;<span'.$cls.'>'.get_entry($ua, 'dezimal').'&nbsp;d</span>&nbsp;';
				else
				{
					$dua=get_entry($ua, 'dezimal');
					$nua='';
					if($err)
						$nua=$_POST['Urlaubsanspruch'][$mc][$ln];
					else if($ud['archiv'] == 0)
						$nua=$dua;
					$txt  = '~<table width="'.$wt.'" border="0" cellpadding="0" cellspacing="0"><tr>';
					$txt .= '<td width="'.$ws.'" nowrap align="right">&nbsp;<i'.$cls.'>('.$dua.'&nbsp;d)</i>&nbsp;&nbsp;</td>';
					$txt .= '<td nowrap align="right">'.fieldText('Urlaubsanspruch['.$mc.']['.$ln.']',$nua,$wf,10,'right').'&nbsp;<span'.$cls.'>d</span>&nbsp;</td>';
					$txt .= '</tr></table>';
				}
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['urlaubsanspruch']=$txt;

				// Übertrag aus Vorjahr (Urlaub_vj: uu)
				$uu=(double)$ud['urlaubsuebertrag_ganzjahr'];
				if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
					$txt='~&nbsp;<span'.$cls.'>'.get_entry($uu, 'dezimal').'&nbsp;d</span>&nbsp;';
				else
				{
					$duu=get_entry($uu, 'dezimal');
					$nuu='';
					if($err)
						$nuu=$_POST['Urlaub_vj'][$mc][$ln];
					else if($ud['archiv'] == 0)
						$nuu=$duu;
					$txt  = '~<table width="'.$wt.'" border="0" cellpadding="0" cellspacing="0"><tr>';
					$txt .= '<td width="'.$ws.'" nowrap align="right">&nbsp;<i'.$cls.'>('.$duu.'&nbsp;d)</i>&nbsp;&nbsp;</td>';
					$txt .= '<td nowrap align="right">'.fieldText('Urlaub_vj['.$mc.']['.$ln.']',$nuu,$wf,10,'right').'&nbsp;<span'.$cls.'>d</span>&nbsp;</td>';
					$txt .= '</tr></table>';
				}
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['urlaub_vj']=$txt;

				// Url. gen. (Urlaub_gn)
				$ug=(double)$ud['urlaub_genommen'];
				$txt='~&nbsp;<span'.$cls.'>'.get_entry($ug, 'dezimal').'&nbsp;d</span>&nbsp;';
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['urlaub_gn']=$txt;

				// Aktiv (Status_Aktiv)
				if($ud['archiv'] == 0)
					$txt=$va[0];	// Gesetzter Wert
				else
					$txt=$va[1];	// Errechneter Wert
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['status_aktiv']='~&nbsp;<span'.$cls.'>'.$txt.'</span>&nbsp;';

				// Hidden
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['personen_id']=$pid;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['pname']=$name;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['artikel_id']=$ud['db'];
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['archiv']=$ud['archiv'];
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_ganzzahl1']=$uj;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_zahl1']=$ua;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_zahl2']=$uu;

				$ln++;
			}

			if(!$dlc)
			{
				$txt='<i class=grey>'.$va[2].'!</i>';	// Keine gesetzten Werte vorhanden
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['jahr']='^class_?" colspan="5" align="center|~<!--'.$txt;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['status_aktiv']='~-->'.$txt;
				$ln++;
			}
		}
	}

	// Leerzeile mit Top Border?
	if($ln)
	{
		fxf_maskColumnBorder(array('Jahr','Urlaubsanspruch','Urlaub_vj','Urlaub_gn','Status_Aktiv'), 1, -1, $mc, $ln, true);
		$ln++;
	}
}

// Buttons ausblenden?
if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
	$Button_Multi_nv=true;
?>