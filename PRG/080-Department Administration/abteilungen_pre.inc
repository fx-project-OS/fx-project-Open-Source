<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : abteilungen_pre.inc                                          //
// Version     : 24.1                                                         //
// Begin       : 2020-10-19                                                   //
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
 * PF 80: Department Administration
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$index=true;
$mc=$GLOBALS['_maskcounter'];

// Height
$Abt_Name_height=22;

// Nowrap Felder
$Abt_Ebene_nw=true;
$Abt_Name_nw=true;
$Abt_Hperson_nw=true;
$Abt_Unterebene_nw=true;
$kostenstelle_id_nw=true;

// Vorhandene Kostenstellenzuordnungen einlesen
$ksz=array();
$kst[0]='&nbsp;<i class="grey">'.meldung(794, true).'</i>';
if($kr_aktiv)
{
	$ksid='';

	$sql="SELECT id, kostenstelle_id FROM kostenzuordnung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND kostenstellenart=920 AND kostendefinition=901";
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$ksz[$t['id']]=$t['kostenstelle_id'];
			if($ksid)
				$ksid .= ',';
			$ksid .= $t['kostenstelle_id'];
		}
	}
fxDebug($ksz, $sql, 3);

	if(strlen((string)$ksid))
	{
		$sql="SELECT kostenstelle_id, kostennummer, kostenbezeichnung FROM kostenstellen WHERE kostenstelle_id IN (".$ksid.") AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$kst[$t['kostenstelle_id']]=$t['kostennummer'];
				if(strlen((string)$t['kostenbezeichnung']))
					$kst[$t['kostenstelle_id']] .= ': '.$t['kostenbezeichnung'];
			}
		}
fxDebug($kst, $sql, 3);
	}
}
else
{
	$kostenstelle_id_nv=true;

	// Adjust remaining field widths for missing cost center field
	$sql  = "SELECT f.id, f.feldname, m.anzeigegroesse1, m.anzeigegroesse2";
	$sql .= " FROM maskenfelder m INNER JOIN felder f ON (f.id=m.eingabefeld)";
	$sql .= " WHERE m.nummer=215 AND m.anzeigeart<>10 AND m.zeile>1 AND (m.anzeigegroesse1<>0 OR m.anzeigegroesse2<>0)";
	$kwa=db_values($sql, '*');
fxDebug($kwa,'$kwa: $sql='.$sql, 3);
	if(fxIsArray($kwa))
	{
		$kc=0;
		$kw=0;
		foreach($kwa as $ka)
		{
			if($ka['id'] == 1609)
				$kw=max(abs($ka['anzeigegroesse1']),abs($ka['anzeigegroesse2']));
			else
				$kc++;
		}

		if($kc && $kw)
		{
			$kw += $kc*6;
			foreach($kwa as $ka)
			{
				if($ka['id'] != 1609)
				{
					$fw=max(abs($ka['anzeigegroesse1']),abs($ka['anzeigegroesse2']));
					if($fw)
						$GLOBALS[$ka['feldname'].'_width']=(int)($fw+floor($kw/$kc));
				}
			}
		}
	}
}

// No right border in the following column
fxf_maskColumnBorder(array('Checkbox'), 0, -1, $mc);

// Firma
$fname='<i class="grey">&uarr;&nbsp;'.db_value("SELECT pname FROM personen WHERE personentyp=".FXP_PT_CLIENT." AND mandanten_id=".$GLOBALS['fxpglobals']['client']).'</i>';
$GLOBALS['fxptdata']['lvalues'][$mc][0]=array(
	'checkbox'			=> '~|',
	'abt_ebene'			=> '~<div style="position:relative;display:inline-block;"><img src="'.$GLOBALS['gfxpath'].'mt_st.png" border="0" style="position:absolute;left:0;top:-16px;"></div>',
	'abt_name'			=> '~'.$fname,
	'abt_hperson'		=> '~',
	'abt_unterebene'	=> '~',
	'kostenstelle_id'	=> '~'
);

// Vorhandene Abteilungen einlesen und prüfen
$sql="SELECT abt_id, abt_ebene, zu_abteilung, abt_name, abt_hperson FROM abteilung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY abt_ebene, abt_name";
$abt=db_values($sql, '*');
if(fxIsArray($abt))
{
	$da=array();
	$cd=0;
	foreach($abt as $an => $av)
	{
		$di=$av['abt_id'];
		if(!$di)
		{
			$ndi=id_max('abteilung', 'abt_id', '', $GLOBALS['fxpglobals']['client']);
			$sql="UPDATE abteilung SET abt_id=".$ndi." WHERE abt_id=".$di." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$aerr=db_x($sql);
			if(!$aerr)
			{
				$di=$ndi;
				$abt[$an]['abt_id']=$di;
			}
		}

		$pd=$av['zu_abteilung'];
		if(!$pd)
			$cd++;

		$da[$di]=array($an,$pd);
	}
fxDebug($da, '$da', 3);

	// Problem beheben, falls keine Abteilung zur Firma zugeordnet ist
	if(!$cd)
	{
		$sql="UPDATE abteilung SET zu_abteilung=0 WHERE AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$aerr=db_x($sql);
		if(!$aerr)
		{
			foreach($abt as $an => $av)
				$abt[$an]['zu_abteilung']=0;
		}
	}
	// Zirkelbeziehungen suchen und beheben
	else
	{
		foreach($da as $di => $pad)
		{
			$an=$pad[0];
			$pd=$pad[1];
			$ds='['.$pd.']>['.$di.']';
			$ec=0;
			$cf=0;
			while(($pd > 0) && ($ec < 20))
			{
				$npd=$da[$pd][1];
				$dp=strpos($ds,'['.$npd.']');
				if($dp !== false)
				{
					$cf=$di;
					break;
				}
				$ds='['.$npd.']>'.$ds;

				$pd=$npd;
				$ec++;
			}
//echo('$cf='.$cf.' -- $ds='.$ds.'<br>');

			// Zirkelbeziehung gefunden!
			if(($cf > 0) || ($ec >= 20))
			{
				$sql="UPDATE abteilung SET zu_abteilung=0 WHERE abt_id=".$di." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$aerr=db_x($sql);
				if(!$aerr)
				{
					$da[$di][1]=0;
					$abt[$an]['zu_abteilung']=0;
				}
			}
		}
	}
}
fxDebug($abt, $sql, 3);

// Vorhandene Abteilungen sortieren
$sabt=array();
if(fxIsArray($abt))
{
	// Startbedingungen
	$level=0;
	$aarr=array(0=>'');

	while(fxIsArray($aarr))
	{
		$naarr=array();
		// Alle Einträge mit der gleichen Abteilungs-Ebene herausfinden
		foreach($aarr as $aid => $acnt)
		{
			$cnt=1;
			foreach($abt as $an => $av)
			{
				if($av['zu_abteilung'] == $aid)
				{
					$ablv=$acnt.substr('000'.$cnt, -3);
					$sabt[$ablv]=$av;

					$naarr[$av['abt_id']]=$ablv;
					$cnt++;

					$abt[$an]['zu_abteilung']=-666;
				}
			}
		}

		$aarr=$naarr;
		$level++;
	}
	ksort($sabt, SORT_STRING);
}
$abt=$sabt;
fxDebug($abt, '$abt', 2);

// Auswahlfeld "Abteilungsleiter"
$flt=filter_person('and', true, false, 'Abt_Hperson');
$Abt_Hperson_arg=array('modus'=>2, 'ptpa'=>array('persontypes'=>array(FXP_PT_EMPLOYEE), 'all_clients'=>false, 'noclient'=>true, 'filter'=>$flt));
$Abt_Hperson_array=auswahl_Personen_ID(1, array(), 'Abt_Hperson');

// Auswahlfeld "Übergeordnete Abteilung"
$da=getDepartments();
fxDebug($da, '$da', 3);
$Abt_Unterebene_array=array();
if(sizeof($da))
{
	foreach($da as $k => $d)
	{
		$ls=str_repeat('&nbsp;', ((strlen((string)$k)/3)-1)*4);
		$Abt_Unterebene_array[$d['departmentid']]=$ls.$d['level'].': '.$d['name'];
	}
}

// Auswahlfeld "Kostenstelle"
$kostenstelle_id_arg=array('kst_def'=>901, 'kst_art'=>"0,920");

// Checkbox ausblenden
if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
	$Checkbox_nv=true;

$linakt=' <i class="lightergrey">{'.strtolower((string)get_text(1391, '20')).'}</i>';	// {inaktiv}

// Listenwerte vorbelegen
$abtleiter=array();
$li=1;
if(fxIsArray($abt))
{
	foreach($abt as $aln => $a)
	{
		// Abt_ID (Hidden)
		$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_id']=$a['abt_id'];

		// Checkbox
		if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['checkbox']='~<span class="grey">'.$li.'</span>';
		else if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['checkbox']='~|';
		else
		{
			$Checkbox_sf=true;
			if($err && $_POST['Checkbox'][$mc][$li])
				$GLOBALS['fxptdata']['lvalues'][$mc][$li]['checkbox']=1;
		}

		// Abteilungs-Nr.
		if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
		{
			$lv=(strlen((string)$aln)/3)-1;
			$tx='';
			// Level
			for($tl=0; $tl<$lv; $tl++)
			{
				$nxtln=substr('000'.(1+(int)substr((string)$aln, $tl*3, 3)), -3);
				$nxtnr=substr((string)$aln, 0, $tl*3).$nxtln;
				if($abt[$nxtnr])
					$tx .= '<div style="position:relative;margin-right:16px;display:inline-block;"><img src="'.$GLOBALS['gfxpath'].'mt_l0.png" border="0" style="position:absolute;left:0;top:-16px;"></div>';
				else
					$tx .= '<div style="position:relative;margin-right:16px;display:inline-block;"></div>';
			}
			$nxtln=substr('000'.(1+(int)substr((string)$aln, $lv*3, 3)), -3);
			$nxtnr=substr((string)$aln, 0, $lv*3).$nxtln;
			if($abt[$nxtnr])
				$tx .= '<div style="position:relative;margin-right:16px;display:inline-block;"><img src="'.$GLOBALS['gfxpath'].'mt_l1.png" border="0" style="position:absolute;left:0;top:-16px;"></div>';
			else
				$tx .= '<div style="position:relative;margin-right:16px;display:inline-block;"><img src="'.$GLOBALS['gfxpath'].'mt_l2.png" border="0" style="position:absolute;left:0;top:-16px;"></div>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_ebene']='~'.$tx.'&nbsp;'.$a['abt_ebene'];
		}
		else if($err)
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_ebene']=$_POST['Abt_Ebene'][$mc][$li];
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_ebene']=$a['abt_ebene'];

		// Abteilungsname
		if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_name']='~'.$a['abt_name'];
		else if($err)
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_name']=$_POST['Abt_Name'][$mc][$li];
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_name']=$a['abt_name'];

		// Abteilungsleiter
		if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
		{
			if($a['abt_hperson'])
			{
				if(!isset($abtleiter[$a['abt_hperson']]))
				{
					$dpa=selGetStyle($Abt_Hperson_array[$a['abt_hperson']]);
fxDebug($dpa,'$dpa', 3);
					if(fxIsArray($dpa))
					{
						$bp=strpos($dpa['tx'],'(');
						if($bp)
							$dpa['tx']=trim(substr((string)$dpa['tx'],0,$bp));
						$abtleiter[$a['abt_hperson']]='<span tooltip="'.$dpa['tt'].'">'.$dpa['tx'].'</span>';
					}
					else
					{
						$sql="SELECT pname, vorname, inaktiv FROM personen WHERE personen_id=".$a['abt_hperson']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$ali=db_values($sql);
						$abtleiter[$a['abt_hperson']]=$ali['pname'];
						if($ali['vorname'])
							$abtleiter[$a['abt_hperson']] .= ', '.$ali['vorname'];
						if($ali['inaktiv'])
							$abtleiter[$a['abt_hperson']]='<span class="grey">'.$abtleiter[$a['abt_hperson']].'</span>'.$linakt;
					}
				}
				$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_hperson']='~'.$abtleiter[$a['abt_hperson']];
			}
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_hperson']='~';
		}
		else if($err)
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_hperson']=$_POST['Abt_Hperson'][$mc][$li];
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_hperson']=$a['abt_hperson'];

		// Übergeordnete Abt.
		if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
		{
			if($a['zu_abteilung'] && isset($Abt_Unterebene_array[$a['zu_abteilung']]))
				$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_unterebene']='~'.$Abt_Unterebene_array[$a['zu_abteilung']];
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_unterebene']='~'.$fname;
		}
		else if($err)
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_unterebene']=$_POST['Abt_Unterebene'][$mc][$li];
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_unterebene']=$a['zu_abteilung'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_id_alias']=$a['zu_abteilung'];

		// Kostenstelle
		if($kr_aktiv)
		{
			$aid=(int)$a['abt_id'];
			if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
			{
				if(fxIsArray($ksz) && $ksz[$aid])
					$kid=$ksz[$aid];
				else
					$kid=0;
				if($kid && $kst[$kid])
					$GLOBALS['fxptdata']['lvalues'][$mc][$li]['kostenstelle_id']='~<span class="grey">&darr;</span>&nbsp;'.$kst[$kid];
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$li]['kostenstelle_id']='~'.$kst[0];
			}
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$li]['kostenstelle_id']=$ksz[$aid];
		}

		$li++;
	}
}

if($li)
{
	fxf_maskColumnBorder(array('Checkbox|0','Abt_Ebene', 'Abt_Name', 'Abt_Hperson', 'Abt_Unterebene', 'kostenstelle_id'), 1, -1, $mc, $li, true);
	$li++;
}

// Neuanlage
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	$headline='+';
	$GLOBALS['fxptdata']['lvalues'][$mc][$li]['checkbox']='~|';
	$GLOBALS['fxptdata']['lvalues'][$mc][$li]['name_feiertag']=$headline;
	if($kr_aktiv)
		fxf_listSectHeader($mc, $li, '&nbsp;<font class="s4b">+</font>', 'abt_ebene', 'kostenstelle_id', 5, -2, 1);
	else
		fxf_listSectHeader($mc, $li, '&nbsp;<font class="s4b">+</font>', 'abt_ebene', 'abt_unterebene', 4, -2, 1);
	$li++;

	$maxli=$GLOBALS['fxpglobals']['settings']['listenneu'];
	if($maxli < 1)
		$maxli=1;
	$maxli += $li;

	while($li < $maxli)
	{
		$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_id']=-1;
		$GLOBALS['fxptdata']['lvalues'][$mc][$li]['checkbox']='~<b class=h3>+</b>';
		if($err)
		{
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_ebene']=$_POST['Abt_Ebene'][$mc][$li];
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_name']=$_POST['Abt_Name'][$mc][$li];
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_hperson']=$_POST['Abt_Hperson'][$mc][$li];
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['abt_unterebene']=$_POST['Abt_Unterebene'][$mc][$li];
			if($kr_aktiv)
				$GLOBALS['fxptdata']['lvalues'][$mc][$li]['kostenstelle_id']=$_POST['kostenstelle_id'][$mc][$li];
		}
		$li++;
	}

	fxf_maskColumnBorder(array('Checkbox|0','Abt_Ebene', 'Abt_Name', 'Abt_Hperson', 'Abt_Unterebene', 'kostenstelle_id'), 1, -1, $mc, $li, true);
	$li++;
}
?>