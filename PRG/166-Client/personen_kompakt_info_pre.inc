<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_info_pre.inc                                //
// Version     : 21.2                                                         //
// Begin       : 2020-10-05                                                   //
// Last Change : 2020-10-05                                                   //
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
 * PF 166: Client etc. - Person info (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if($_pid && $_cid)
{
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]=array();

	// Steering variables
	$index=true;
	$maske387_nch=true;
	$dbconv=true;

	// Literals
	$lit_id=get_text(179,'5');
	$lit_nummer=get_text(742,'10');
	$lit_status=get_text(859,'10');
	$lit_stelle=get_text(23,'10');

	$lit_anrede=get_text(17,'10');
	$lit_titel=get_text(25,'10');
	$lit_zusatz=get_text(16,'10');

	// Reference texts
	$aktiv='<span class="green">'.meldung(300, true).'</span>';
	$inaktiv='<span class="red">'.meldung(299, true).'</span>';
	$sprung=meldung(1697,false);
	$stammkostenstelle=meldung(804,false);
	$anschriftsdaten=meldung(2518,false);
	$kommunikationsdaten=meldung(2098,false);

	// Reference arrays
	$anrarr=fxf_getReferenceValues(17);
	$posarr=fxf_getReferenceValues(23);
	$titarr=fxf_getReferenceValues(25);
	$typarr=fxf_getReferenceValues(30);
	$lkzarr=fxf_getReferenceValues(63);
	$adrarr=fxf_getReferenceValues(95);
	$komarr=fxf_getReferenceValues(94);

	// Program function rights
	$plinks=array(274=>166, 2090=>164, 275=>167, 840=>168, 276=>169, 620=>170, 1372=>171, 841=>172, 2091=>173, 2092=>174);

	// Person display array
	// ...selected
	$persarr=array($_pid=>0);
	// ...superior
	if($_cid == $GLOBALS['fxpglobals']['client'])
	{
		$sql="SELECT elter FROM personen WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
		$pid=(int)db_value($sql);
		if($pid)
			$persarr[$pid]=1;
	}
	// ...inferior
	if($_cid == $GLOBALS['fxpglobals']['client'])
	{
		$sql="SELECT personen_id FROM personen WHERE elter=".$_pid." AND mandanten_id=".$_cid;
		if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fperson_inaktiv'] && !$GLOBALS['fxpglobals']['filter']['inaktiv'])
			$sql .= " AND (inaktiv=0 OR inaktiv IS NULL)";
		$sql .= " ORDER BY personentyp, pname, vorname";
		$tmp=db_values($sql,'*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
				$persarr[(int)$t['personen_id']]=2;
		}
	}
fxDebug($persarr,'$persarr', 3);

	// Display parameter
	$tdtemp='<td nowrap class="darkgrey">{lit}:</td><td width="30%" style="box-shadow:1px 1px 2px rgba(0,0,0, 0.10) inset;">{val}</td>';
	$dsparr=array(
		array('bgcolor'=>'79aec7', 'isize'=>48, 'fsize'=>6, 'padd'=>''),
		array('bgcolor'=>'969696', 'isize'=>32, 'fsize'=>5, 'padd'=>'&nbsp;&nbsp;&nbsp;<b class="s5">&uarr;</b>'),
		array('bgcolor'=>'b4b4b4', 'isize'=>32, 'fsize'=>4, 'padd'=>'&nbsp;&nbsp;&nbsp;<b class="s5">&darr;</b>')
	);

	// Cycle through all displayable persons
	$am=$GLOBALS['_maskcounter'];
	$az=0;
	foreach($persarr as $pid => $pt)
	{
		// ...Empty line?
		if($az)
		{
			$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='~|';
			$az++;
		}

		// Person info
		$sql  = "SELECT personentyp, pname, vorname, personen_initialen, name_zusatz, anrede, perstitel, pposition, geschlecht, abt_id, inaktiv";
		$sql .= " FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$_cid;
		$infarr=db_values($sql);
fxDebug($infarr,$sql, 3);

		// ...Header - Name
		$dtr=$plinks[$infarr['personentyp']];
		$plr=tr_rechte($dtr);
		if($pt && $plr && !isset($GLOBALS['ltt_'.$dtr]))
			$GLOBALS['ltt_'.$dtr]='<b>'.$sprung.'</b>: '.trPath($dtr);
//$arr[$i] .= fieldLink('link',$linktxt,'?pid='.$P_ID.'&ptyp='.$P_arr['personentyp'].'&aktion=1',$GLOBALS['ltt_'.$dtr],$dtr);
		$name=$infarr['pname'];
		if(strlen($infarr['vorname']))
			$name .= ', '.$infarr['vorname'];
		$fc='ffffff';
		if($infarr['inaktiv'])
			$fc='dddddd';
		$txt  = '<div style="position:relative;text-align:left;">&nbsp;'.person_pic($pid, $dsparr[$pt]['isize'], $infarr['geschlecht'], $_cid, false, 'border-top-left-radius:6px;vertical-align:middle;', true).'&nbsp;&nbsp;<b class="s'.$dsparr[$pt]['fsize'].'" style="color:#'.$fc.';text-shadow:2px 2px 2px #000000;">'.$name.'</b>'.$dsparr[$pt]['padd'];
		$txt .= '<div style="position:absolute;top:4px;right:8px;"><i class="s3 black">'.$typarr[$infarr['personentyp']].'</i></div>';
		$txt .= '</div>';
		if($pt && $plr)
			$txt=fieldLink('link',$txt,'?pid='.$pid.'&ptyp='.$infarr['personentyp'].'&aktion=1',$GLOBALS['ltt_'.$dtr],$dtr);
		$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_'.$dsparr[$pt]['bgcolor'].'" nowrap style="margin:0;padding:6px 2px;border:0;border-top-left-radius:12px;border-top-right-radius:12px;|~'.$txt;
		$az++;

		// ...Line: ID/Initials, Status + Job Desc.
		$tab = '<table width="100%" border=0 cellpadding=2 cellspacing=2><tr>';
		$in='<span class="lightblue">'.$pid.'</span>';
		if(strlen($infarr['personen_initialen']))
			$in .= '&nbsp;<span class="lightergrey">/</span>&nbsp;'.$infarr['personen_initialen'];
		$tab .= strtr($tdtemp, array('{lit}'=>$lit_id.'/'.$lit_nummer, '{val}'=>$in));
		$tab .= '<td nowrap>&nbsp;&nbsp;&nbsp;</td>';
		$st=$aktiv;
		if($infarr['inaktiv'])
			$st=$inaktiv;
		$tab .= strtr($tdtemp, array('{lit}'=>$lit_status, '{val}'=>$st));
		$tab .= '<td nowrap>&nbsp;&nbsp;&nbsp;</td>';
		$tab .= strtr($tdtemp, array('{lit}'=>$lit_stelle, '{val}'=>$posarr[$infarr['pposition']]));
		// ...Line: Salutat., Title + Name affix.
		$tab .= '</tr><tr>';
		$tab .= strtr($tdtemp, array('{lit}'=>$lit_anrede, '{val}'=>$anrarr[$infarr['anrede']]));
		$tab .= '<td nowrap>&nbsp;&nbsp;&nbsp;</td>';
		$tab .= strtr($tdtemp, array('{lit}'=>$lit_titel, '{val}'=>$titarr[$infarr['perstitel']]));
		$tab .= '<td nowrap>&nbsp;&nbsp;&nbsp;</td>';
		$tab .= strtr($tdtemp, array('{lit}'=>$lit_zusatz, '{val}'=>$infarr['name_zusatz']));
		$tab .= '</tr></table>';
		$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$tab;
		$az++;

		// Get base cost accountings (901) of the person (908)
		$sql  = "SELECT s.kostennummer, s.kostenbezeichnung FROM kostenstellen s, kostenzuordnung z";
		$sql .= " WHERE s.kostenstelle_id=z.kostenstelle_id AND s.mandanten_id=".$_cid." AND z.id=".$pid." AND z.kostenstellenart=908";
		$sql .= " AND z.kostendefinition=901 AND z.mandanten_id=".$_cid." AND s.id_sprache=".$GLOBALS['fxpglobals']['lang'];
		$arr=db_values($sql);
fxDebug($arr,$sql, 3);
		if(fxIsArray($arr))
		{
			$txt='<div style="margin-left:16px;margin-top:12px;padding:4px 8px;background:#e1e1e1;border:0;border-top-left-radius:8px;border-top-right-radius:8px;"><b class="s4">'.$stammkostenstelle.'</b></div>';
			$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
			$az++;

			$val='<b>'.$arr['kostennummer'].'</b>';
			if(strlen($arr['kostenbezeichnung']))
				$val .= '&nbsp;&nbsp;'.$arr['kostenbezeichnung'];
			$txt='<div style="margin-left:16px;padding:8px 0;padding-left:16px;border-left:1px solid #f1f1f1;border-right:1px solid #f1f1f1;">'.$val.'</div>';
			$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
			$az++;

			$txt='<div style="margin-left:16px;padding:2px 12px;background:#e1e1e1;border:1px solid #e8e8e8;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;"></div>';
			$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
			$az++;
		}

		// Address data
		$sql="SELECT adresstyp, strasse, landkz, plz, ort FROM adressen WHERE personen_id=".$pid." AND mandanten_id=".$_cid." ORDER BY plz";
		$arr=db_values($sql, '*');
fxDebug($arr,$sql, 3);
		if(fxIsArray($arr))
		{
			$txt='<div style="margin-left:16px;margin-top:12px;padding:4px 8px;background:#e1e1e1;border:0;border-top-left-radius:8px;border-top-right-radius:8px;"><b class="s4">'.$anschriftsdaten.'</b></div>';
			$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
			$az++;

			$at=0;
			foreach($adrarr as $adt => $adn)
			{
				$tb='';
				$ac=0;
				foreach($arr as $a)
				{
					if($a['adresstyp'] == $adt)
					{
						// ...Address type
						if($at != $adt)
						{
							$at=$adt;
							$ac++;

							$txt='<div style="margin-left:16px;padding:2px 8px;background:#f1f1f1;"><b class="s3">&middot;&nbsp;'.$adn.'</b></div>';
							$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
							$az++;

							$lkz='';
							if(isset($lkzarr[$a['landkz']]))
								$lkz=$lkzarr[$a['landkz']].'-';

							$ml='';
							if($ac > 1)
								$ml='margin-left:16px;';

							$tb .= '<div style="float:left;width:278px;'.$ml.'padding:4px;border:1px solid #e1e1e1;box-shadow:2px 2px 3px rgba(0,0,0, 0.15);">';
							$tb .= $name.'<br />'.$a['strasse'].'<br />';
							$tb .= '<b>'.$lkz.$a['plz'].'</b> '.$a['ort'];
							$tb .= '</div>';
						}
					}
				}

				if(strlen($tb))
				{
					$txt='<div style="margin-left:16px;height:58px;padding:8px 0;padding-left:16px;border-left:1px solid #f1f1f1;border-right:1px solid #f1f1f1;">'.$tb.'</div>';
					$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
					$az++;
				}
			}

			$txt='<div style="margin-left:16px;padding:2px 12px;background:#e1e1e1;border:1px solid #e8e8e8;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;"></div>';
			$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
			$az++;
		}

		// Communication data
		$sql="SELECT kommunikationstyp, kommunikation_txt, anmerkung, zeitstempel FROM kommunikation WHERE personen_id=".$pid." AND mandanten_id=".$_cid." ORDER BY ";
		if(isset($_POST['ksort']) && ($_POST['ksort'] == 'c'))
			$sql .= "zeitstempel, ";
		$sql .= "kommunikation_txt";
		$arr=db_values($sql, '*');
fxDebug($arr,$sql, 3);
		if(fxIsArray($arr))
		{
			$txt='<div style="margin-left:16px;margin-top:12px;padding:4px 8px;background:#e1e1e1;border:0;border-top-left-radius:8px;border-top-right-radius:8px;"><b class="s4">'.$kommunikationsdaten.'</b></div>';
			$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
			$az++;

			// Sort data by date or type
			$sarr=array();
			if(isset($_POST['ksort']) && ($_POST['ksort'] == 'c'))
			{
				foreach($arr as $k => $a)
					$sarr[]=$k;
			}
			else
			{
				foreach($komarr as $kot => $kon)
				{
					foreach($arr as $k => $a)
					{
						if($a['kommunikationstyp'] == $kot)
							$sarr[]=$k;
					}
				}
			}
fxDebug($sarr,'$sarr', 3);

			$bga=array('ffffff', 'fcfcfc');

			$kt='';
			$kc=0;
			foreach($sarr as $k)
			{
				$a=$arr[$k];
				$t=$a['kommunikationstyp'].'|';
				if(isset($_POST['ksort']) && ($_POST['ksort'] == 'c'))
					$t .= $a['zeitstempel'];

				// ...New type
				if($kt != $t)
				{
					$kt=$t;
					$kc=0;

					$zs='';
					if(isset($_POST['ksort']) && ($_POST['ksort'] == 'c'))
						$zs='<div class="s2 normal grey" style="float:right;">'.get_entry($a['zeitstempel'],'datetime').'</div>';

					$txt='<div style="margin-left:16px;padding:2px 8px;background:#f1f1f1;"><b class="s3">&middot;&nbsp;'.$komarr[$a['kommunikationstyp']].$zs.'</b></div>';
					$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
					$az++;
				}
				else
					$kc++;

				$w=906;

				$bt='';
				if($kc)
					$bt='border-top:1px dotted #f1f1f1;';

				$bg='background:#'.$bga[$kc%2].';';

				// ...Display by type
				$tl=strtolower($a['kommunikation_txt']);
				if(strpos($tl,'@') || ($a['kommunikationstyp'] == 2366))	// Email
					$val='<img src="'.$GLOBALS['gfxpath'].'eml_b_20x20.png" align="top" style="width:18px;height:18px;">&nbsp;&nbsp;<a class="fxlink" href="mailto:'.$a['kommunikation_txt'].'" target="_blank">'.$a['kommunikation_txt'].'</a>';
				else if((substr($tl,0,4) == 'http') || (substr($tl,0,3) == 'ftp') || (substr($tl,0,3) == 'www') || ($a['kommunikationstyp'] == 2367))	// Link
				{
					$val='<img src="'.$GLOBALS['gfxpath'].'glb_b_20x20.png" align="top" style="width:18px;height:18px;">&nbsp;&nbsp;<a class="fxlink" href="';
					if((substr($tl,0,4) != 'http') && (substr($tl,0,3) != 'ftp'))
						$val .= 'https://';
					$val .= $a['kommunikation_txt'].'" target="_blank">'.$a['kommunikation_txt'].'</a>';
				}
				else if(is_phone($a['kommunikation_txt']) || ($a['kommunikationstyp'] == 2365))	// Phone
					$val='<img src="'.$GLOBALS['gfxpath'].'con_b_20x20.png" align="top" style="width:18px;height:18px;">&nbsp;&nbsp;<span class="blue">'.$a['kommunikation_txt'].'</span>';
				else
					$val=$a['kommunikation_txt'];
				$txt  = '<div style="width:'.$w.'px;margin-left:16px;padding:8px 0;padding-left:16px;'.$bg.$bt.'border-left:1px solid #f1f1f1;border-right:1px solid #f1f1f1;">';
				$txt .= '<div style="white-space:pre-wrap;word-wrap:break-word;overflow:hidden;">'.$val.'</div>';
				if(strlen($a['anmerkung']))
					$txt .= ' <div style="margin:8px 8px 0 16px;padding:8px;font-family:\'Courier New\', Courier, sans-serif;color:#888888;white-space:pre;box-shadow:1px 1px 2px rgba(0,0,0, 0.10) inset;overflow-x:auto;overflow-y:hidden;">'.$a['anmerkung'].'</div>';
				$txt .= '</div>';
				$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
				$az++;
			}

			$txt='<div style="margin-left:16px;padding:2px 12px;background:#e1e1e1;border:1px solid #e8e8e8;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;"></div>';
			$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
			$az++;
		}

		$txt='<div style="margin-left:16px;padding:2px 12px;height:8px;"></div>';
		$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_ffffff" style="border:0;margin:0;padding:0;|~'.$txt;
		$az++;

		$GLOBALS['fxptdata']['lvalues'][$am][$az]['uebersicht']='^col_'.$dsparr[$pt]['bgcolor'].'" nowrap style="margin:0;padding:4px 24px;border:1px solid #e8e8e8;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;|~';
		$az++;
	}

	unset($dbconv);
}
?>