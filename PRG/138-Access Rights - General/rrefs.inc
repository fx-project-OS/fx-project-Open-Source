<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rrefs.inc                                                    //
// Version     : 21.2                                                         //
// Begin       : 2020-12-21                                                   //
// Last Change : 2020-12-21                                                   //
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
 * PF 138: Access Rights - General
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
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

$callAJAX=false;
if(!isset($GLOBALS['eid']))
	$GLOBALS['eid']='';
if(fxIsArray($_GET) && isset($_GET['eid']))
{
	$GLOBALS['eid']=$_GET['eid'];
	$callAJAX=true;
}

$rrpa=array('rcat', 'rfld', 'rid');
foreach($rrpa as $rrp)
{
	if(fxIsArray($_GET) && isset($_GET[$rrp]))
		$GLOBALS[$rrp]=(int)$_GET[$rrp];
	else if(!isset($GLOBALS[$rrp]))
		$GLOBALS[$rrp]=0;
}

// Auswahlarray füllen
if($GLOBALS['fxpglobals']['tr'] == 107)	// Dokumente suchen
{
	$rbit=2;	// Anzeigen
	$elter_art_array=array();
	$elter_art_id_array=array();
}
else
{
	$rbit=4;	// Anlegen
	$Rechte_ReferenzID_array=array();
	$ID_Referenz_array=array();
}
if($rcat > 0)
{
	if($GLOBALS['fxpglobals']['tr'] == 107)	// Dokumente suchen
	{
		$rfarr=array(0=>'<i>-'.meldung(1422,true).'-</i>');
		if($GLOBALS['eid'] == 'elter_art')
			$s_name='elter_art_id';
		else
			$s_name='';
	}
	else
	{
		$rfarr=array(0=>'<i>'.meldung(684,true).'</i>');
		if($GLOBALS['eid'] == 'Rechte_Ref')
			$s_name='Rechte_ReferenzID';
		else if($GLOBALS['eid'] == 'Rechte_ReferenzID')
			$s_name='ID_Referenz';
		else
			$s_name='';
	}

	if((!$callAJAX || ($s_name == 'Rechte_ReferenzID') || ($s_name == 'elter_art')) && $rcat)
	{
		switch($rcat)
		{
			case 35:	// Reports
				$sql  = "SELECT repid, repname";
				$sql .= " FROM repliste WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
				// GF darf für alle Reports Rechte vergeben - alle anderen nur für die, welche sie selbst angelegt haben oder zu denen sie Neuanlagerechte haben...
				if(!$GLOBALS['fxpglobals']['persdat']['roles'][2])
				{
					$bgr=get_benutzergruppen();
					$abt=get_abteilungen();

					$sql .= " AND (aenderungs_id=".$GLOBALS['fxpglobals']['person']." OR repid IN (";
					$sql .= "SELECT id_referenz FROM rechte_referenz WHERE rechte_ref=35 AND (aktionscode & ".$rbit.")>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$sql .= " AND ((gruppeart=2163 AND benutzergr_id=".$GLOBALS['fxpglobals']['user'].")";
					if(fxIsArray($bgr))
						$sql .= " OR (gruppeart=2164 AND benutzergr_id IN (".array_to_csv($bgr)."))";
					if(fxIsArray($abt))
						$sql .= " OR (gruppeart=2165 AND benutzergr_id IN (".array_to_csv($abt)."))";
					$sql .= ")))";
				}
				$sql .= " ".db_orderby('repname');
				$arr=db_values($sql, '*');
				if(fxIsArray($arr))
				{
					$rfarr[-1]='<d>';

					foreach($arr as $a)
						$rfarr[$a['repid']]='<img src='.$GLOBALS['gfxpath'].'fld_2.png align=top />&nbsp;'.$a['repname'];
				}
			break;

			case 36:	// Personen
				// ...Int./Ext. Mitarbeiter anzeigen? (Je nach Mandanteneinstellungswert)
				$vpt=FXP_PT_CLIENT.", ".FXP_PT_CLIENT_CP.", ".FXP_PT_CUSTOMER.", ".FXP_PT_CUSTOMER_CP.", ".FXP_PT_PARTNER.", ".FXP_PT_PARTNER_CP.", ".FXP_PT_CONTACT.", ".FXP_PT_CONTACT_CP;
				$dma=strtolower(substr($GLOBALS['fxpglobals']['settings']['persrecht_ma'],0,1));
				if($dma === 'j')
					$vpt .= FXP_PT_EMPLOYEE.", ".FXP_PT_CONTRACTOR;
				$sql="SELECT DISTINCT p.personentyp".filter_person('from')." AND personentyp IN (".$vpt.")";
				$pta=db_values($sql, '*');
				if(fxIsArray($pta))
				{
					$sql="SELECT wertetabid, tabwert FROM wertetab WHERE wertetabid IN (".array_to_csv($pta, 'personentyp').") AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY tabwert";
					$pwa=db_values($sql, '*');
					if(fxIsArray($pwa))
					{
						$rfarr[-1]='<d>';

						foreach($pwa as $a)
							$rfarr[$a['wertetabid']]='<img src='.$GLOBALS['gfxpath'].'fld_2.png align=top />&nbsp;'.$a['tabwert'];
					}
				}
			break;

			case 37:	// Projekte: Hauptprojekte
				$sql  = "SELECT p.projekt_id, p.vorgangsnummer, p.name_projekt";
				$sql .= " FROM projekte p, zeitdaten z WHERE z.zeit_id=p.zeit_id AND z.zeitart=102";
				// GF darf für alle Projekte Rechte vergeben - alle anderen nur für die, welche sie selbst angelegt haben oder zu denen sie Neuanlagerechte haben
				if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
					$sql .= " AND (p.aenderungs_id=".$GLOBALS['fxpglobals']['person']." OR p.projekt_id IN (".prights_get(0,$rbit)."))";
				if($GLOBALS['fxpglobals']['persdat']['personentyp'] == 620)
				{
					$GLOBALS['ignore_kunde']=true;
					$sql .= " AND p.kunde=".(int)$GLOBALS['fxpglobals']['persdat']['elter'];
				}
				$sql .= filter_projekt('and', true, false, 'dummy');
				$sql .= " ORDER BY p.vorgangsnummer";
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					$rfarr[-1]='<d>';

					// Alle Hauptprojekte ermitteln
					$rna=array();
					foreach($tmp as $t)
					{
						$id=(int)$t['projekt_id'];
						$no=substr($t['vorgangsnummer'],0,4);
						if(strlen($t['vorgangsnummer']) == 4)
							$rna[$no]=array('id'=>$id, 'name'=>$t['name_projekt'], 'right'=>'');
						else if(!isset($rna[$no]))
							$rna[$no]=array('id'=>$id, 'name'=>"", 'right'=>'<i></i>');
					}
					// Evtl. Infos nachladen, falls noch nicht vorhanden
					$lpi='';
					foreach($rna as $no => $ra)
					{
						if(!strlen($ra['name']))
							$lpi .= " OR vorgangsnummer='".$no."'";
					}
					if(strlen($lpi))
					{
						$sql="SELECT projekt_id, vorgangsnummer, name_projekt FROM projekte WHERE (".substr($lpi,4).") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$tmp2=db_values($sql,'*');
//fxDebug($tmp2,$sql);
						if(fxIsArray($tmp2))
						{
							foreach($tmp2 as $t)
								$rna[$t['vorgangsnummer']]=array('id'=>$t['projekt_id'], 'name'=>$t['name_projekt'], 'right'=>'<i></i>');
						}
					}
					// Array sortieren nach Projektname?
					if(strtolower($GLOBALS['fxpglobals']['settings']['zeit_projektnr']) != 'ja')
					{
						$rnas=array();
						foreach($rna as $no => $ra)
							$rnas[strtolower($ra['name']).'|'.$ra['id']]=array('id'=>$ra['id'], 'no'=>$no, 'name'=>$ra['name'], 'right'=>$ra['right']);
						ksort($rnas,SORT_STRING);
						foreach($rnas as $ra)
							$rfarr[$ra['id']]='<img src='.$GLOBALS['gfxpath'].'fld_2.png align=top />&nbsp;'.$ra['name'].'&nbsp;&nbsp;['.no_null($ra['no']).']'.$ra['right'];
					}
					else
					{
						foreach($rna as $no => $ra)
							$rfarr[$ra['id']]='<img src='.$GLOBALS['gfxpath'].'fld_2.png align=top />&nbsp;'.no_null($no).':&nbsp;&nbsp;'.$ra['name'].$ra['right'];
					}
				}
			break;

			case 38:	// Dokumente: Hauptordner
				// Personen-ID dieses Mandanten
				$sql="SELECT personen_id FROM personen WHERE personentyp=".FXP_PT_CLIENT." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$cpi=(int)db_value($sql);

				$sql  = "SELECT v.dok_verlauf_id, d.elter_art, a.elter_art_name";
				$sql .= " FROM d_dokumente d, d_dok_verlauf v, d_dok_elt_art a";
				// Alle aktiven Dokumente ausser 1="Allgemein", 3="E-Mails" und 6/MID="Personen/Mandant" (in der Rechtetransaktion)
				$sql .= " WHERE v.dok_id=d.dok_id AND d.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND a.elter_art_id=d.elter_art AND a.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND v.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND";
				$sql .= " (v.aktiv_kz=1 AND d.elter_art IS NOT NULL";
				if($GLOBALS['fxpglobals']['tr'] != 107)	// Ausser "Dokumente suchen"
					$sql .= " AND d.elter_art>1 AND d.elter_art<>3 AND (d.elter_art<>6 OR (d.elter_art=6 AND d.elter<>".$cpi."))";
				// GF darf für alle Dokumente Rechte vergeben - alle anderen nur für die, welche sie selbst angelegt haben oder zu denen sie Neuanlagerechte haben...
				if(!$GLOBALS['fxpglobals']['persdat']['roles'][2])
				{
					$bgr=get_benutzergruppen();
					$abt=get_abteilungen();

					$sql .= " AND (d.angelegt_von=".$GLOBALS['fxpglobals']['person']." OR d.dok_id IN (";
					$sql .= "SELECT id_referenz FROM rechte_referenz WHERE rechte_ref=38 AND (aktionscode & ".$rbit.")>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$sql .= " AND ((gruppeart=2163 AND benutzergr_id=".$GLOBALS['fxpglobals']['user'].")";
					if(fxIsArray($bgr))
						$sql .= " OR (gruppeart=2164 AND benutzergr_id IN (".array_to_csv($bgr)."))";
					if(fxIsArray($abt))
						$sql .= " OR (gruppeart=2165 AND benutzergr_id IN (".array_to_csv($abt)."))";
					$sql .= ")))";
				}
				$sql .= ")";
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					$rfarr[-1]='<d>';

					$ra=array();
					$aa=array();
					foreach($tmp as $t)
					{
						$id=(int)$t['dok_verlauf_id'];
						$ea=(int)$t['elter_art'];
						if(!isset($rfarr[$id]) && !isset($aa[$ea]))
						{
							$efld=meldung($t['elter_art_name'],false);
							$aa[$ea]=$efld;
							$ra[$efld]=$ea;
						}
					}
					ksort($ra);
					foreach($ra as $efld => $id)
						$rfarr[$id]='<img src='.$GLOBALS['gfxpath'].'fld_2.png align=top />&nbsp;'.$efld;
				}
			break;
		}

		if(!$callAJAX)
		{
			if($GLOBALS['fxpglobals']['tr'] == 107)	// Dokumente suchen
			{
				$elter_art_array=$rfarr;
				$maskenfeldwerte['elter_art']=$rfld;
			}
			else
			{
				$Rechte_ReferenzID_array=$rfarr;
				$maskenfeldwerte['rechte_referenzid']=$rfld;
			}
		}
	}

	if((!$callAJAX || ($s_name == 'ID_Referenz') || ($s_name == 'elter_art_id')) && $rcat && $rfld)
	{
		if($GLOBALS['fxpglobals']['tr'] == 107)	// Dokumente suchen
			$rfarr=array(0=>'<i>'.meldung(794,true).'</i>');
		else
			$rfarr=array(0=>'<i>'.meldung(684,true).'</i>');

		switch($rcat)
		{
			case 36:	// Personen
				$sql="SELECT p.personen_id, p.pname, p.vorname, p.geschlecht".filter_person('from')." AND p.personentyp=".$rfld." ORDER BY p.pname, p.vorname";
				$pna=db_values($sql, '*');
				if(fxIsArray($pna))
				{
					$rfarr[-1]='<d>';

					$pmxh=256;
					foreach($pna as $a)
					{
						$pers_icons=getPersonIcons($p['personen_id'], $pmxh, (int)$p['geschlecht'], $rfld);
//fxDebug($pers_icons,'$pers_icons: '.$p['personen_id']);
						$img='';
						if(!$pers_icons['def'])
						{
							if(strlen($pers_icons['256']))
								$img .= '<tt><img src='.$pers_icons['256'].$pers_icons['mtime'].' style=margin-top:4px;border-radius:4px; /></tt>';
							else if(strlen($pers_icons['src']))
								$img .= '<tt><img src='.$pers_icons['src'].$pers_icons['mtime'].$pers_icons['size_src'].' style=max-height:'.$pmxh.'px;width:auto;margin-top:4px;border-radius:4px; /></tt>';
						}
						$op='';
						if($pers_icons['def'])
							$op='opacity:0.1;';
						$img .= '<img src='.$pers_icons['ico'].$pers_icons['mtime'].' align=top width=14 height=14 style=\'border:1px solid #d2d2d2;border-radius:2px;'.$op.'\' />&nbsp;';
						$name=$a['pname'];
						if(strlen($a['vorname']))
							$name .= ', '.$a['vorname'];
						$rfarr[$a['personen_id']]=$img.$name;
					}
				}
			break;

			case 37:	// Projekte: Hauptprojekt, Unterprojekte und Unteraufgaben
				$sql  = "SELECT p.projekt_id, p.vorgangsnummer, p.name_projekt, p.projektart, p.projekt_status";
				$sql .= " FROM projekte p, zeitdaten z WHERE z.zeit_id=p.zeit_id AND z.zeitart=102 AND p.mutterprojekt_id=".$rfld;
				// GF darf für alle Projekte Rechte vergeben - alle anderen nur für die, welche sie selbst angelegt haben oder zu denen sie Neuanlagerechte haben
				if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
					$sql .= " AND (p.aenderungs_id=".$GLOBALS['fxpglobals']['person']." OR p.projekt_id IN (".prights_get(0,$rbit)."))";
				if($GLOBALS['fxpglobals']['persdat']['personentyp'] == 620)
				{
					$GLOBALS['ignore_kunde']=true;
					$sql .= " AND p.kunde=".(int)$GLOBALS['fxpglobals']['persdat']['elter'];
				}
				$sql .= filter_projekt('and', true, false, 'dummy');
				$sql .= " ORDER BY p.vorgangsnummer";
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					$rfarr[-1]='<d>';

					// Alle Hauptprojekte ermitteln
					$lpi='';
					$rna=array();
					foreach($tmp as $t)
					{
						$id=(int)$t['projekt_id'];
						$lv=floor(strlen($t['vorgangsnummer'])/5);
						$rna[$t['vorgangsnummer']]=array('id'=>$id, 'name'=>$t['name_projekt'], 'level'=>$lv, 'type'=>$t['projektart'], 'status'=>$t['projekt_status'], 'right'=>'');
						if($lv)
						{
							$nv=substr($t['vorgangsnummer'],0,-5);
							while(strlen($nv))
							{
								if(isset($rna[$nv]))
									break;
								$lpi .= " OR vorgangsnummer='".$nv."'";

								$nv=substr($nv,0,-5);
							}
						}
					}
					// Evtl. Infos nachladen, falls noch nicht vorhanden
					if(strlen($lpi))
					{
						$sql="SELECT projekt_id, vorgangsnummer, name_projekt, projektart, projekt_status FROM projekte WHERE (".substr($lpi,4).") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$tmp2=db_values($sql,'*');
//fxDebug($tmp2,$sql);
						if(fxIsArray($tmp2))
						{
							foreach($tmp2 as $t)
							{
								$lv=floor(strlen($t['vorgangsnummer'])/5);
								$rna[$t['vorgangsnummer']]=array('id'=>$t['projekt_id'], 'name'=>$t['name_projekt'], 'level'=>$lv, 'type'=>$t['projektart'], 'status'=>$t['projekt_status'], 'right'=>'<d>');
							}
						}
						ksort($rna,SORT_STRING);
					}
					// Array ausgeben
					foreach($rna as $no => $ra)
					{
						$name=no_null($no).':&nbsp;&nbsp;'.$ra['name'];
						if($ra['type'] == FXP_PRJ_MAIN)
						{
							$img=$GLOBALS['gfxpath'].'fld_2.png align=top';
							if(strtolower($GLOBALS['fxpglobals']['settings']['zeit_projektnr']) != 'ja')
								$name=$ra['name'].'&nbsp;&nbsp;['.no_null($no).']';
						}
						else if($ra['type'] == FXP_PRJ_SUB)
							$img=$GLOBALS['gfxpath'].'fld_3.png align=top';
						else
							$img=$GLOBALS['gfxpath'].'lt'.$ra['status'].'.png align=bottom';
						$rfarr[$ra['id']]=str_repeat('&nbsp;', $ra['level']*5).'<img src='.$img.' />&nbsp;'.$name.$ra['right'];
					}
				}
			break;

			case 38:	// Dokumente: Unterordner + Dokumente
				// We need dms class
				if(!class_exists('fxpDoc'))
				{
					$GLOBALS['__includer']['dms']=true;
					require('includer.inc');
				}
				if(class_exists('fxpDoc'))
					$doc=new fxpDoc();
				else
					$doc=false;

				// Personen-ID dieses Mandanten
				$sql="SELECT personen_id FROM personen WHERE personentyp=".FXP_PT_CLIENT." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$cpi=(int)db_value($sql);

				$sql  = "SELECT d.dok_id, d.elter_art, d.elter, d.dok_art, v.dok_verlauf_id, v.datei_name, v.dateigroesse, v.version_nr";
				$sql .= " FROM d_dokumente d, d_dok_verlauf v";
				// Alle aktiven Dokumente 6/MID="Personen/Mandant"
				$sql .= " WHERE v.dok_id=d.dok_id AND d.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND v.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND";
				$sql .= " (v.aktiv_kz=1 AND d.elter_art=".$rfld;
				if($rfld == 6)
					$sql .= " AND d.elter<>".$cpi;
				// GF darf für alle Dokumente Rechte vergeben - alle anderen nur für die, welche sie selbst angelegt haben oder zu denen sie Neuanlagerechte haben...
				if(!$GLOBALS['fxpglobals']['persdat']['roles'][2])
				{
					$bgr=get_benutzergruppen();
					$abt=get_abteilungen();

					$sql .= " AND (d.angelegt_von=".$GLOBALS['fxpglobals']['person']." OR d.dok_id IN (";
					$sql .= "SELECT id_referenz FROM rechte_referenz WHERE rechte_ref=38 AND (aktionscode & ".$rbit.")>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$sql .= " AND ((gruppeart=2163 AND benutzergr_id=".$GLOBALS['fxpglobals']['user'].")";
					if(fxIsArray($bgr))
						$sql .= " OR (gruppeart=2164 AND benutzergr_id IN (".array_to_csv($bgr)."))";
					if(fxIsArray($abt))
						$sql .= " OR (gruppeart=2165 AND benutzergr_id IN (".array_to_csv($abt)."))";
					$sql .= ")))";
				}
				$sql .= ")";
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					$rfarr[-1]='<d>';
					$oarr=array();
					$nc=-2;
					$li=0;
					foreach($tmp as $t)
					{
						$id=(int)$t['dok_id'];
						if(!isset($ra[$id]))
						{
							$ra[$id]=true;
							$fn=$t['datei_name'];

							$pt='';
							$sp='';
							if($doc && ($t['elter'] > 0))
							{
								$pa=$doc->getDocPath($rfld.'_'.$t['elter'], $t);
//fxDebug($pa,'$pa: '.$rfld.'_'.$t['elter']);
								$pe=explode($doc->fs, $pa['full']);
								if(sizeof($pe) > 1)
								{
									$ie=explode('/', $pa['id']);
									$iu='';
									foreach($pe as $pc => $p)
									{
										if($pc)
										{
											if(strlen($pa['sort']))
												$pt=strtolower($pa['sort'].'_'.$pc);
											else
											{
												if(strlen($pt))
													$pt .= $doc->fs;
												$pt .= strtolower($p);
											}
											if(strlen($iu))
												$iu .= '/';
											if(strpos($ie[0],'_') !== false)
												$iu .= $ie[$pc-1];
											else
												$iu .= $ie[$pc];
											$oarr[$pt]=array('id'=>$iu, 'sp'=>$sp, 'fn'=>$p, 'd'=>true);
											$sp .= '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
										}
									}
								}
							}

							// Dokument
							if($GLOBALS['fxpglobals']['tr'] != 107)	// Ausser "Dokumente suchen"
								$oarr[$pt.$fn]=array('id'=>$id, 'sp'=>$sp, 'fn'=>$fn, 'd'=>false);
						}
					}

					ksort($oarr,SORT_STRING);
//fxDebug($oarr,'$oarr');
					foreach($oarr as $ofn => $oa)
					{
						if(!$oa['d'])
						{
							$rfarr[$oa['id']]=$oa['sp'].$oa['fn'];
							$li=$oa['id'];
						}
						else
						{
							if($li > 0)
							{
								$rfarr[$nc]='<d>';
								$nc--;
							}
							if($GLOBALS['fxpglobals']['tr'] == 107)	// Dokumente suchen
								$rfarr[$oa['id']]=$oa['sp'].'<img src='.$GLOBALS['gfxpath'].'fld_3.png align=top />&nbsp;'.$oa['fn'];
							else
								$rfarr[$nc]=$oa['sp'].'<img src='.$GLOBALS['gfxpath'].'fld_3.png align=top />&nbsp;'.$oa['fn'].'<d>';
							$li=$nc;
							$nc--;
						}
					}
				}
			break;
		}

		if(!$callAJAX)
		{
			if($GLOBALS['fxpglobals']['tr'] == 107)	// Dokumente suchen
			{
				$elter_art_id_array=$rfarr;
				$maskenfeldwerte['elter_art_id']=$rid;
			}
			else
			{
				$ID_Referenz_array=$rfarr;
				$maskenfeldwerte['id_referenz']=$rid;
			}
		}
	}

	// Ergebnis im AJAX-Fall am Bildschirm ausgeben
	if($callAJAX)
	{
		$result=selEntriesJS($s_name,$rfarr,false,false,true);
//echo('<pre>'.fxHtmlEncode($result).'</pre>');
		echo($result);
	}
}
?>