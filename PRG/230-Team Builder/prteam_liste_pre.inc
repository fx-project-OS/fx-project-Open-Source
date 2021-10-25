<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prteam_liste_pre.inc                                         //
// Version     : 21.2                                                         //
// Begin       : 2020-11-16                                                   //
// Last Change : 2020-11-16                                                   //
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
 * PF 230: Team Builder
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

setForm(true);

// Alle anzuzeigenden Personen werden hier gespeichert
$pers_arr=array();
$mskfilter=0;

// Button im Anzeigemodus ausblenden
$Button_Multi_nv=false;
if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
	$Button_Multi_nv=true;

// Projektteam-MA immer anzeigen
if($_POST['projektteam_id'])
{
	$sql="SELECT personen_id, leiter FROM projektteam WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id>0 AND wertetabid=".$_POST['projektteam_id'];
	$pt_pers=db_values($sql, '*');
	if(fxIsArray($pt_pers))
	{
		foreach($pt_pers as $pp)
		{
			$pid=(int)$pp['personen_id'];

			$pers_arr[$pid]['val'] |= 1;
			$pers_arr[$pid]['ldr']=(int)$pp['leiter'];
		}
	}
}

// Alle internen (275) und externen (840) MA (Filter berücksichtigen)
if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
{
	$GLOBALS['ignore_personentyp']=true;

	$sql="SELECT p.personen_id FROM personen p WHERE p.personen_id>0 AND (p.personentyp=".FXP_PT_EMPLOYEE." OR p.personentyp=".FXP_PT_CONTRACTOR.")".filter_person('and', true, false, 'Pname');
	$rs_pers=db_values($sql, '*');
	if(fxIsArray($rs_pers))
	{
		foreach($rs_pers as $pp)
		{
			$pid=(int)$pp['personen_id'];

			$pers_arr[$pid]['val'] |= 4;
		}
	}

	$maske468_flt=$GLOBALS['Pname_flt'];
}

// Zugeordnete MA zu Projekt
if($_POST['Projekt_ID'])
{
	$inf=inferior_vn($_POST['Projekt_ID']);
	if(strlen($inf))
	{
		$sql="SELECT personen_id FROM maplanung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id>0 AND projekt_id IN (".$inf.")";
		$pr_pers=db_values($sql, '*');
		if(fxIsArray($pr_pers))
		{
			foreach($pr_pers as $pp)
			{
				$pid=(int)$pp['personen_id'];
				if($pid > 0)
					$pers_arr[$pid]['val'] |= 2;
			}
		}
	}
}

// Zeitspanne ermitteln
$von='';
$bis='';
if($_POST['Projekt_ID'])
{
	$sql="SELECT soll_beginn, soll_ende, ist_beginn, ist_ende FROM zeitdaten WHERE projekt_id=".$_POST['Projekt_ID']." AND zeitart=".FXP_TE_PROJECT." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$zd=db_values($sql);
	if(fxIsArray($zd))
	{
		$von=$zd['soll_beginn'];
		if(strlen($zd['ist_beginn']) && ($zd['ist_beginn'] < $von))
			$von=$zd['ist_beginn'];
		$bis=$zd['soll_ende'];
		if(strlen($zd['ist_ende']) && ($zd['ist_ende'] > $bis))
			$bis=$zd['ist_ende'];
	}
}
if(!$von)
{
	// Startdatum=Starttag der aktuellen Woche
	$von=fxNow();

	$tag 	= (int)substr($von, 6, 2);
	$monat	= (int)substr($von, 4, 2);
	$jahr 	= (int)substr($von, 0, 4);
	$wtg=fx_date('w', fx_mktime(0, 0, 0, $monat, $tag, $jahr));
	$stg=(int)($GLOBALS['fxpglobals']['settings']['ersterwt']);
	$von=fx_date('Ymd000000', fx_mktime(0, 0, 0, $monat, $tag-$wtg+$stg, $jahr));

	// Enddatum=Endtag nach der Standard-Zeitspanne (4 Wochen)
	$jahr	= (int)(substr($von, 0, 4));
	$monat	= (int)(substr($von, 4, 2));
	$tag	= (int)(substr($von, 6, 2));
	$bis=fx_date('Ymd000000', fx_mktime(0, 0, 0, $monat, $tag+$GLOBALS['fxpglobals']['settings']['stdzeitspanne']-1, $jahr));
}
$von=substr($von, 0, 8).'000000';
$bis=substr($bis, 0, 8).'240000';

// Personen-Infos holen
$pers=array();
if(fxIsArray($pers_arr))
{
	$pers_csv=array_to_csv($pers_arr, true);

	$sql  = "SELECT p.personen_id, p.pname, p.vorname, p.geschlecht, p.personentyp, p.inaktiv,";
	$sql .= " m.maeintrittsdatum, m.maaustrittsdatum, m.wochenarbeitstd, m.arbeitstage, m.soll_beginn, m.pause, m.montag AS b, m.dienstag AS c, m.mittwoch AS d, m.donnerstag AS e, m.freitag AS f, m.samstag AS g, m.sonntag AS a";
	$sql .= " FROM personen p, madaten m";
	$sql .= " WHERE m.personen_id=p.personen_id AND p.personen_id IN (".$pers_csv.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " ORDER BY p.pname, p.vorname";
	$pers=db_values($sql, '*');
}

// Liste aufbereiten
if(fxIsArray($pers))
{
	if($_POST['Button_Submit'])
	{
		unset($_POST['Checkbox']);
		unset($_POST['leiter']);
		unset($_POST['oldcheck']);
		unset($_POST['oldleiter']);
	}

	$maske468_flt=$mskfilter;
	$index468=true;
	$jetzt=fxNow();

	$recht205=tr_rechte(205);

	// NoWrap
	$Pname_nw=true;
	$WochenArbeitStd_nw=true;
	$Datum_nw=true;

	// Texte
	$tt_int=meldung(351, false);	// Int. Mitarbeiter
	$tt_ext=meldung(352, false);	// Ext. Mitarbeiter
//fxDebug($pers, 'Personen');

	$wochentage=array();
	$warr=db_values("SELECT position, tabwert FROM wertetab WHERE id_feld=289 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang'], '*');
	if(fxIsArray($warr))
	{
		foreach($warr as $wa)
			$wochentage[$wa['position']]=substr($wa['tabwert'], 0, 2);
	}

	$wta=array();
	if(sizeof($wochentage))
	{
		$ewt=(int)$GLOBALS['fxpglobals']['settings']['ersterwt'];
		for($i=0; $i<7; $i++)
		{
			$zch=chr($ewt+97);
			$wta[$zch]=$wochentage[$ewt];
			$ewt++;
			if($ewt > 6)
				$ewt=0;
		}
	}

	// Mask variables
	$mc=$GLOBALS['_maskcounter'];
	$z=0;
	$mca=array('Checkbox|0', 'Pname', 'extern', 'leiter', 'Auslastung', 'WochenArbeitStd', 'Datum');
	fxf_maskColumnBorder(array('Checkbox'), 0, -1, $mc);

	if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
		$show_name=true;
	else
		$show_name=false;

	$inakt=' <i class="grey normal">{'.get_text(1391, '20').'}</i>';	// {inaktiv}
	foreach($pers as $p)
	{
		// Vor Heute ausgetretene MA auf inaktiv setzen
		if($p['maaustrittsdatum'] && ($p['maaustrittsdatum'] < $jetzt))
			$p['inaktiv']=1;

		// Inaktive MA ausblenden, ausser sie sind im Projektteam
		if($p['inaktiv'] && !($pers_arr[$p['personen_id']]['val'] & 1))
			continue;

		// Namenszeile
		if($show_name)
		{
			$show_name=false;

			$oldname=$_POST['old_pteam_name'];
			if($_POST['pteam_name'] && $_POST['Button_Multi'])
				$name=$_POST['pteam_name'];
			else if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
				$name=meldung($projektteam_id, false);
			else if($_POST['Projekt_ID'])
				$name='PT: '.db_value("SELECT name_projekt FROM projekte WHERE projekt_id=".$_POST['Projekt_ID']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			else
				$name='Team';
			$txt  = '<b class="lit">'.get_text(1586, '15').'</b>&nbsp;<i class="lit">('.get_entry($von, 'datum').'&nbsp;-&nbsp;'.get_entry($bis, 'datum').')</i><br />';
			$txt .= fieldText('pteam_name',$name, 896, 255).fieldHidden('old_pteam_name',$oldname);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='~|';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pname']='^col_e4e4e4" nowrap colspan="6" style="padding:4px;border-right:1px solid #'.$GLOBALS['msk_tbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_tbcol'].';"><!--|~&nbsp;';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~-->'.$txt;
			$z++;
		}

		// Checkbox
		if($_POST['Button_Multi'])
		{
			if($_POST['Checkbox'][$mc][$z])
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']=1;
		}
		else if($pers_arr[$p['personen_id']]['val'] & 1)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']=1;
		else if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) && ($pers_arr[$p['personen_id']]['val'] & 2))
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']=1;

		// Farbe
		if($p['inaktiv'])
			$cls='red bold';
		else if(($pers_arr[$p['personen_id']]['val'] & 1) && !$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox'])
			$cls='blue bold';
		else if(!$p['wochenarbeitstd'])
			$cls='lightred';
		else if($GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox'])
			$cls='black bold';
		else
			$cls='black';
		if($pers_arr[$p['personen_id']]['ldr'])
			$cls .= ' underline';

		// Name
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pname']='^class_? '.$cls.'|~';
		$txt=person_pic($p['personen_id']);
		if($recht205)
			$txt=fieldLink('hrinfo'.$z, $txt, 'loader.php?url=prs_info.inc&pid='.$p['personen_id']);
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pname'] .= $txt;

		// Fähigkeiten
		$tt_fae='';
		$sql  = "SELECT af.maart_id, a.maart_name, af.mafaehigkeit_id, f.mafaehigkeit_name";
		$sql .= " FROM ma_art_faehigkeiten af, ma_art a, ma_faehigkeit f";
		$sql .= " WHERE a.maart_id=af.maart_id AND a.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND f.mafaehigkeit_id=af.mafaehigkeit_id AND f.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
		$sql .= " AND af.personen_id=".$p['personen_id']." AND a.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND f.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND af.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ORDER BY a.maart_name, f.mafaehigkeit_name";
		$fa=db_values($sql, '*');
		if(fxIsArray($fa))
		{
			$tt_fae .= '<br /><hr class=fxhr><br />';
			$mnb='';
			foreach($fa as $f)
			{
				if($f['maart_name'] != $mnb)
				{
					$mnb=$f['maart_name'];
					$tt_fae .= $nl.$mnb;
				}
				$tt_fae .= '<br />&nbsp;&nbsp;&nbsp;&middot;&nbsp;'.$f['mafaehigkeit_name'];
			}
		}
		// Intern/Extern
		if($p['personentyp'] == FXP_PT_EMPLOYEE)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['extern']='~<div class="darkgrey" tooltip="<b>'.$tt_int.'</b>'.$tt_fae.'" style="cursor:help;height:18px;border:1px solid #aaaaaa;border-radius:6px;">'.strtolower(substr($tt_int, 0, 1)).'</div>';
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['extern']='~<div class="blue" tooltip="<b>'.$tt_ext.'</b>'.$tt_fae.'" style="cursor:help;height:18px;border:1px solid #aaaaaa;border-radius:6px;">'.strtolower(substr($tt_ext, 0, 1)).'</div>';

		// Leiter
		if($_POST['Button_Multi'])
		{
			if($_POST['leiter'][$mc][$z])
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['leiter']=1;
		}
		else if($pers_arr[$p['personen_id']]['ldr'])
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['leiter']=1;

		// Auslastung ermitteln und anzeigen
		$ausl=personen_auslastung($von, $bis, $p['personen_id']);
		if(!$ausl['ausl'])
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['auslastung']='~'.get_entry(0, 'dezimal').'&nbsp;%&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'au_ud.png" style="width:12px;height:18px;border:1px solid #aaaaaa;border-radius:4px;" align="middle">';
		else
		{
//fxDebug($ausl, 'Auslastungs-Array: '.$ausl['name']);
			$ap=(int)((10+(int)$ausl['ausl'])/10)*10;
			if($ap > 200)
				$ap='_x';
			else
				$ap=substr('000'.$ap, -3);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['auslastung']='~'.get_entry($ausl['ausl'], 'dezimal').'&nbsp;%&nbsp;&nbsp;<img src="'.$GLOBALS['gfxpath'].'au'.$ap.'.png" style="width:12px;height:18px;border:1px solid #aaaaaa;border-radius:4px;" align="middle">';
		}

		// Wochenarbeitsstunden
		$wst  = '~<table border="0" cellpadding="0" cellspacing="0"><tr>';
		$wst .= '<td align="right">&nbsp;'.get_entry($p['wochenarbeitstd'], 'dezimal').'&nbsp;h&nbsp;&nbsp;</td>';
		if(fxIsArray($wta))
		{
			$wst .= '<td><table border="0" bgcolor="#cccccc" cellpadding="1" cellspacing="1"><tr>';
			foreach($wta as $zch => $tag)
			{
				if($p[$zch])
					$wcol='ffffff';
				else
					$wcol='e4e4e4';
				$wst .= '<td width="20" bgcolor="#'.$wcol.'" align="center"><font class="s2">'.$tag.'</font></td>';
			}
			$wst .= '</tr></table></td>';
		}
		$wst .= '</tr></table>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['wochenarbeitstd']=$wst;

		// Datum
		$dtm='~'.get_entry($p['maeintrittsdatum'], 'datum');
		if($p['maaustrittsdatum'])
			$dtm .= '&nbsp;<font class="grey">-</font>&nbsp;<font class="red">'.get_entry($p['maaustrittsdatum'], 'datum').'</font>';
		// Alten Zuordnungsstatus verstecken
		if($pers_arr[$p['personen_id']]['val'] & 1)
			$dtm .= fieldHidden('oldcheck['.$mc.']['.$z.']',1);
		// Alten Teamleiterstatus verstecken
		if($pers_arr[$p['personen_id']]['ldr'])
			$dtm .= fieldHidden('oldleiter['.$mc.']['.$z.']',1);
		// Namen verstecken
		$name=$p['pname'];
		if(strlen(trim($p['vorname'])))
			$name .= ',&nbsp;'.trim($p['vorname']);
		$dtm .= fieldHidden('pname['.$mc.']['.$z.']',strip_tags($name));
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']=$dtm;

		// Hidden
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['personen_id']=$p['personen_id'];

		$z++;
	}

	if($z)
	{
		fxf_maskColumnBorder($mca, 1, -1, $mc, $z, true);
		$z++;
	}
}
?>