<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : s_exim_tools.inc                                             //
// Version     : 24.1                                                         //
// Begin       : 2020-12-17                                                   //
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
 * PF 237: Export (XML) - Function Collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$xmlths='padding:2px;border-right:1px solid #'.$GLOBALS['msk_tbcol'].';';
$xmltds='padding:1px;paddin-right:0;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';';
$xmlels='border-top:1px solid #'.$GLOBALS['msk_nbcol'].';';
$xmlals='border-top:1px dotted #'.$GLOBALS['msk_nbcol'].';';
$xmlaes='border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';';


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $sname   - Mandatory parameter:                  ???PARAMETER???
 * @param various $sarr    - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $nmisarr - Optional parameter (default=true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function sys_check($sname, $sarr, $nmisarr=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($sarr))
		return;

	$tmp='';
	foreach($sarr as $sid => $stxt)
	{
		if(($nmisarr && $GLOBALS[$sname][$sid]) || (!$nmisarr && $GLOBALS[$sname]))
			$checked='1';
		else
			$checked='';
		$find='';
		if($nmisarr)
			$find='['.$sid.']';
		$tmp .= fieldCheckbox($sname.$find, '', $checked);
		$tmp .= '&nbsp;&nbsp;'.$stxt.'<br>'.$GLOBALS['nl'];
	}

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $grp - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function load_exim_cfg($grp)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Dateinamen festlegen
	$fn=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/cfg_exim'.$grp.'_'.$GLOBALS['fxpglobals']['person'].'_'.$GLOBALS['fxpglobals']['lang'].'.cfg';
	if(!file_exists($fn))
		$fn=$GLOBALS['datpath'].'0/cfg_exim'.$grp.'_'.$GLOBALS['fxpglobals']['lang'].'.cfg';
	if(!file_exists($fn))
		return false;

	// Datei laden
	$far=array();
	$inhalt=file($fn);
	if(fxIsArray($inhalt))
	{
		foreach($inhalt as $zeile)
		{
			$zeile=trim((string)$zeile);
			$zex=explode('^', $zeile);
			$tabname=strtolower((string)$zex[0]);
			if(isset($zex[1]) && ($zex[1] !== NULL))
			{
				$feldname_real=substr((string)$zex[1], 1);
				$feldname=strtolower((string)$feldname_real);
				$feldart=substr((string)$zex[1], 0,1);
			}
			else
			{
				$feldname_real='';
				$feldname='';
				$feldart='';
			}
			if(strlen((string)$feldname))
			{
				if(isset($zex[2]) && ($zex[2] !== NULL))
					$feldid=(int)$zex[2];
				else
					$feldid=0;
				$defaultwert=false;
				if(isset($zex[3]) && ($zex[3] !== NULL))
				{
					$feldtyp=(int)$zex[3];
					$defpos=strpos($zex[3], '_');
					if($defpos !== false)
						$defaultwert=(int)substr((string)$zex[3], $defpos+1);
				}
				else
					$feldtyp=0;
				if(isset($zex[4]) && ($zex[4] !== NULL))
					$xmlelement=$zex[4];
				else
					$xmlelement='';
				if(isset($zex[5]) && ($zex[5] !== NULL))
					$xmlalias=$zex[5];
				else
					$xmlalias='';
				if(isset($zex[6]) && ($zex[6] !== NULL))
					$beschreibung=$zex[6];
				else
					$beschreibung='';
				if(isset($zex[7]) && ($zex[7] !== NULL))
					$beispiel=$zex[7];
				else
					$beispiel='';

				$far[$feldname]=array(
					'name'	=> $feldname_real,
					'id'	=> $feldid,
					'tab'	=> $tabname,
					'art'	=> $feldart,
					'typ'	=> $feldtyp,
					'def'	=> $defaultwert,
					'xml'	=> $xmlelement,
					'alias'	=> $xmlalias,
					'text'	=> $beschreibung,
					'bsp'	=> $beispiel
				);
			}
		}
	}

	return $far;
}

/**
 * ???FUNCTION???
 *
 * @param various $grp - Mandatory parameter: ???PARAMETER???
 * @param various $inh - Mandatory parameter: ???PARAMETER???
 */
function save_exim_cfg($grp, $inh)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$filepath=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/';
	$cerr=fxCreatePath($filepath);
	if(!strlen((string)$cerr))
	{
		$fn=$filepath.'cfg_exim'.$grp.'_'.$GLOBALS['fxpglobals']['person'].'_'.$GLOBALS['fxpglobals']['lang'].'.cfg';
		fxSave($fn, $inh);
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $felder  - Mandatory parameter:                  ???PARAMETER???
 * @param various $data    - Mandatory parameter:                  ???PARAMETER???
 * @param various $tabname - Mandatory parameter:                  ???PARAMETER???
 * @param string  $tb      - Optional parameter (default="		"):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_xml_element($felder, $data, $tabname, $tb="		")
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ea='';
	$ev='';
	foreach($felder as $feldname => $fdat)
	{
		if($fdat['tab'] != $tabname)
			continue;
		if($fdat['art'] == '*')	// Attribut
			$ea .= ' '.$fdat['xml'].'="'.$data[$feldname].'"';
		else
		{
			$xd='';
			if($data[$feldname] !== NULL)
				$xd=trim((string)$data[$feldname]);
			if(strlen((string)$xd))
			{
				if(($fdat['def'] !== NULL) && strlen((string)$fdat['def']))	// Nachschlagewert
				{
					$xd=(int)$xd;
					if(!isset($GLOBALS['wtab']) || !is_array($GLOBALS['wtab']))
						$GLOBALS['wtab']=array();
					if(!isset($GLOBALS['wtab'][$xd]))
						$GLOBALS['wtab'][$xd]=htmlspecialchars(meldung($xd, false), ENT_QUOTES);
					if(strlen((string)$GLOBALS['wtab'][$xd]))
						$ev .= $tb."<".$fdat['xml'].">".$GLOBALS['wtab'][$xd]."</".$fdat['xml'].">".$GLOBALS['nl_b'];
					else
						$ev .= $tb."<".$fdat['xml']." />".$GLOBALS['nl_b'];
				}
				else
					$ev .= $tb."<".$fdat['xml'].">".htmlspecialchars($xd, ENT_QUOTES)."</".$fdat['xml'].">".$GLOBALS['nl_b'];
			}
			else
				$ev .= $tb."<".$fdat['xml']." />".$GLOBALS['nl_b'];
		}
	}

	return array('ea'=>$ea, 'ev'=>$ev);
}
/**
 * ???FUNCTION???
 *
 * @param various $tabname  - Mandatory parameter:                  ???PARAMETER???
 * @param various $ar       - Mandatory parameter:                  ???PARAMETER???
 * @param various &$nw      - Mandatory parameter:                  ???PARAMETER???
 * @param various $felder   - Mandatory parameter:                  ???PARAMETER???
 * @param various $_xmlelem - Mandatory parameter:                  ???PARAMETER???
 * @param string  $persid   - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $mand     - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function import_argumente($tabname, $ar, &$nw, $felder, $_xmlelem, $persid='', $mand=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ra=array();
	if($mand === true)
		$ra['mandanten_id']=$GLOBALS['fxpglobals']['client'];
	else if($mand)
		$ra['mandanten_id']=(int)$mand;

	if(strlen((string)$persid))
		$ra['personen_id']=(int)$persid;

	foreach($felder as $feldname => $f)
	{
		if($f['tab'] != $tabname)
			continue;

		$xml_feldname=$_xmlelem[$feldname];
		$value=$ar[$xml_feldname];

		// Typprüfung
		if(!strlen((string)$value))
			$value='NULL';
		else if(($f['typ'] === 5) && ($f['def'] !== false))	// Nachschlagewerte
		{
			$lvalue=strtolower((string)$value);
			if($nw[$feldname][$lvalue])	// gefunden
				$value=(int)$nw[$feldname][$lvalue];
			else if($f['def'])			// Defaultwert verwenden
				$value=(int)$f['def'];
			else
			{
				$value=convert_string($value,'todb');

				// Alle Sprachen ermitteln
				$sql="SELECT wertetabid FROM wertetab WHERE mandanten_id=0 AND id_feld=53 AND id_sprache=1";
				$sar=db_values($sql,'*');
				if(fxIsArray($sar))
				{
					// WerteTabID festlegen
					$wid=id_max('wertetab', 'wertetabid', '', $GLOBALS['fxpglobals']['client']);

					$nerr=false;
					foreach($sar as $s)
					{
						$spr=$s['wertetabid'];
						$wvalue=$value;
						if($spr != $GLOBALS['fxpglobals']['lang'])
							$wvalue='§'.$wvalue;
						$sql  = "INSERT INTO wertetab";
						$sql .= " (mandanten_id, wertetabid, id_feld, id_sprache, position, tabwert, satzvers, archiv)";
						$sql .= " VALUES";
						$sql .= " (".$GLOBALS['fxpglobals']['client'].", $wid, ".$f['id'].", $spr, 99, '$wvalue', 0, 0)";
//fxDebug($sql, '$sql');
						$nerr=db_x($sql);
						if($nerr)
							break;
					}

					if(!$nerr)
					{
						$nw[$feldname][$lvalue]=$wid;
						$value=(int)$wid;
					}
					else
						$value=0;
				}
				else
					$value=0;
			}
		}
		else if(($f['typ'] == 1) || ($f['typ'] == 3) || ($f['typ'] == 13) || ($f['typ'] == 15))	// Text
			$value=(string)convert_string($value, 'todb');
		else if($f['typ'] == 4)	// Ja/Nein
		{
			if($value)
				$value=1;
			else
				$value=0;
		}
		else if(($f['typ'] == 5) || ($f['typ'] == 12) || ($f['typ'] == 14))	// Ganzzahl
		{
			if($feldname == 'elter')
			{
				$sv=trim((string)$value);
				if(!strlen((string)$sv))
					$value=0;
				else
				{
					$iv=(int)$sv;
					if(((string)$iv === (string)$sv) || !isset($ra['personentyp']) || !isset($GLOBALS['etypes'][$ra['personentyp']]))
						$value=$iv;
					else
					{
						$et=$GLOBALS['etypes'][$ra['personentyp']];
						$sql="SELECT personen_id FROM personen WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personentyp=".$et." AND ".db_like('pname', $sv);
						$value=(int)db_value($sql);
//fxDebug($value,$feldname.' ('.$xml_feldname.') - personentyp='.$ra['personentyp'].', eltertyp='.$et.': '.$ar[$xml_feldname]);
					}
				}
			}
			else
				$value=(int)$value;
		}
		else if(($f['typ'] == 6) || ($f['typ'] == 11))	// Dezimal
			$value=get_double($value);
		else if(strlen((string)$value) == 14)	// DateTime gegeben
			$value=(string)$value;
		else	// DateTime unbekannt
			$value=(string)validate_entry($value, $verr, $f['typ']);

		// NotNULL Felder
		if(($value == 'NULL') && ($f['art'] != '-'))
		{
			if(($f['typ'] == 1) || ($f['typ'] == 3) || ($f['typ'] == 13) || ($f['typ'] == 15))	// Text
				$value=$GLOBALS['unknown'];
			else if(($f['typ'] == 7) || ($f['typ'] == 9) || ($f['typ'] == 10))	// DateTime
				$value=(string)fxNow();
			else
				$value=0;
		}

		if($f['art'] == '*')	// Primärschlüssel
			$feldname='*'.$feldname;

		$ra[$feldname]=$value;
	}
//fxDebug($ra,'$ra: $tabname='.$tabname);

	return $ra;
}

/**
 * ???FUNCTION???
 *
 * @param various $tabname   - Mandatory parameter:                  ???PARAMETER???
 * @param various $ra        - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $retstatus - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function check_dublicate($tabname, $ra, $retstatus=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Primärschlüssel
	$pk='';
	foreach($ra as $feldname => $f)
	{
		if(substr((string)$feldname, 0, 1) == '*')
		{
			$pk=$feldname;
			break;
		}
	}

	if($pk)
	{
		$sn=substr((string)$pk,1);
		$sv=(int)$ra[$pk];
	}

	if($sv)
	{
		$sql="SELECT ".$sn." FROM ".$tabname." WHERE ".$sn."=".$sv." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tid=db_value($sql);
//fxDebug($sql, 'Ergebnis 1: ['.$tid.']');
		if($tid)
		{
			if($retstatus)
			{
				$GLOBALS['dublicate_id']=$tid;
				return 1;	// Dublette wegen Primärschlüssel (ID)
			}
			else
				return $tid;
		}
	}

	$sql='';
	if($tabname == 'personen')
	{
		$sql="SELECT ".$sn." FROM ".$tabname." WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
		if($ra['pname'] != 'NULL')
			$sql .= " AND ".db_like('pname', $ra['pname']);
		else
			$sql .= " AND pname IS NULL";
		if($ra['vorname'] != 'NULL')
			$sql .= " AND ".db_like('vorname', $ra['vorname']);
		else
			$sql .= " AND vorname IS NULL";
		if($ra['personentyp'])
			$sql .= " AND personentyp=".$ra['personentyp'];
		else
			$sql .= " AND (personentyp IS NULL OR personentyp=0)";
		if($ra['geschlecht'])
			$sql .= " AND geschlecht=".$ra['geschlecht'];
		else
			$sql .= " AND (geschlecht IS NULL OR geschlecht=0)";
		if($ra['elter'])
			$sql .= " AND elter=".$ra['elter'];
		else
			$sql .= " AND (elter IS NULL OR elter=0)";
//fxDebug($ra['elter'],'elter');
	}
	else if($tabname == 'adressen')
	{
		$sql="SELECT ".$sn." FROM ".$tabname." WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id=".$ra['personen_id'];
		if($ra['adresstyp'] != 'NULL')
			$sql .= " AND adresstyp=".$ra['adresstyp'];
		else
			$sql .= " AND adresstyp IS NULL";
		if($ra['landkz'] != 'NULL')
			$sql .= " AND landkz=".$ra['landkz'];
		else
			$sql .= " AND landkz IS NULL";
		if($ra['strasse'] != 'NULL')
			$sql .= " AND ".db_like('strasse', $ra['strasse']);
		else
			$sql .= " AND strasse IS NULL";
		if($ra['plz'] != 'NULL')
			$sql .= " AND ".db_like('plz', $ra['plz']);
		else
			$sql .= " AND plz IS NULL";
	}
	else if($tabname == 'kommunikation')
	{
		$sql="SELECT ".$sn." FROM ".$tabname." WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id=".$ra['personen_id'];
		if($ra['kommunikationstyp'] != 'NULL')
			$sql .= " AND kommunikationstyp=".$ra['kommunikationstyp'];
		else
			$sql .= " AND kommunikationstyp IS NULL";
		if($ra['kommunikation_txt'] != 'NULL')
			$sql .= " AND ".db_like('kommunikation_txt', $ra['kommunikation_txt']);
		else
			$sql .= " AND kommunikation_txt IS NULL";
	}

	if($sql)
	{
		$tida=db_values($sql,'*');
//fxDebug($tida, 'Ergebnis 2: '.$sql);
		if(fxIsArray($tida))
		{
			if($retstatus)
			{
				$rtid='';
				foreach($tida as $t)
				{
					if($rtid)
						$rtid .= ',';
					$rtid .= $t[$sn];
				}
				$GLOBALS['dublicate_id']=$rtid;
				return 2;	// Dublette wegen Namen in DB
			}
			else
				return $tida[0][$sn];
		}
	}

	if(!$retstatus)
		return 0;

	if(fxIsArray($GLOBALS['dupl_imp']) && fxIsArray($GLOBALS['dupl_imp'][$tabname]))
	{
		foreach($GLOBALS['dupl_imp'][$tabname] as $d)
		{
			if(($tabname == 'personen') && (($ra['pname'] == $d['pname']) && ($ra['vorname'] == $d['vorname']) && ($ra['personentyp'] == $d['personentyp']) && ($ra['elter'] == $d['elter']) && ($ra['geschlecht'] == $d['geschlecht'])))
			{
//fxDebug($d, 'Ergebnis 3-1');
				return 3;	// Dublette wegen Namen in Import-Datei
			}
			else if(($tabname == 'adressen') && (($ra['adresstyp'] == $d['adresstyp']) && ($ra['landkz'] == $d['landkz']) && ($ra['strasse'] == $d['strasse']) && ($ra['plz'] == $d['plz'])))
			{
//fxDebug($d, 'Ergebnis 3-2');
				return 3;
			}
			else if(($tabname == 'kommunikation') && (($ra['kommunikationstyp'] == $d['kommunikationstyp']) && ($ra['kommunikation_txt'] == $d['kommunikation_txt'])))
			{
//fxDebug($d, 'Ergebnis 3-3');
				return 3;
			}
		}
	}

	// Keine Dublette, dann Datensatz zum Vergleich merken
	if(($tabname == 'personen') || ($tabname == 'adressen') || ($tabname == 'kommunikation'))
	{
		if(!isset($GLOBALS['dupl_imp'][$tabname]))
			$GLOBALS['dupl_imp'][$tabname]=array();

		if($tabname == 'personen')
			$GLOBALS['dupl_imp'][$tabname][]=array('pname'=>$ra['pname'], 'vorname'=>$ra['vorname'], 'personentyp'=>$ra['personentyp'], 'geschlecht'=>$ra['geschlecht']);
		else if($tabname == 'adressen')
			$GLOBALS['dupl_imp'][$tabname][]=array('adresstyp'=>$ra['adresstyp'], 'landkz'=>$ra['landkz'], 'strasse'=>$ra['strasse'], 'plz'=>$ra['plz']);
		else if($tabname == 'kommunikation')
			$GLOBALS['dupl_imp'][$tabname][]=array('kommonukationstyp'=>$ra['kommunikationstyp'], 'kommunikation_txt'=>$ra['kommunikation_txt']);
	}

	return 0;
}

/**
 * ???FUNCTION???
 *
 * @param various $tabname - Mandatory parameter:                ???PARAMETER???
 * @param various $id      - Mandatory parameter:                ???PARAMETER???
 * @param various $elter   - Mandatory parameter:                ???PARAMETER???
 * @param various $fname   - Mandatory parameter:                ???PARAMETER???
 * @param various $size    - Mandatory parameter:                ???PARAMETER???
 * @param various $ra      - Mandatory parameter:                ???PARAMETER???
 * @param various $apid    - Mandatory parameter:                ???PARAMETER???
 * @param various $adtm    - Mandatory parameter:                ???PARAMETER???
 * @param string  $cnid    - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $ccid    - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function import_db($tabname, $id, $elter, $fname, $size, $ra, $apid, $adtm, $cnid='', $ccid='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$err=false;
	$serr=false;

	$tabid=0;
	$status=887;
	$old='';
	$modus=0;

	// Primärschlüssel
	$pk='';
	foreach($ra as $feldname => $f)
	{
		if(substr((string)$feldname, 0, 1) == '*')
		{
			$pk=$feldname;
			break;
		}
	}

	if($pk)
	{
		$sn=substr((string)$pk, 1);
		$sv=(int)$ra[$pk];
		if($tabname == 'personen')
			$GLOBALS['epid']=$sv;
	}

	$tid=(int)check_dublicate($tabname, $ra, false);

	// $cnid -> Neuanlage
	// $ccid -> Ändern
	if($cnid || $ccid)
	{
		$modus=1;		// Neuanlage
		if($tid && !$cnid && $ccid)
			$modus=2;	// Ändern

		// Elter besonders handeln
		if($ra['elter'] && ($ra['elter'] != 'NULL'))
		{
			$eid=$ra['elter'];
			if(fxIsArray($GLOBALS['elter']) && $GLOBALS['elter'][$tabname][$eid])
				$eid=$GLOBALS['elter'][$tabname][$eid];

			$sql="SELECT ".$sn." FROM ".$tabname." WHERE ".$sn."=".$eid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$teid=db_value($sql);
//fxDebug($sql, 'Ergebnis Elter: '.$teid);
			if($teid)
				$ra['elter']=(int)$teid;
			else // Falls kein Elter gefunden, dann zum aktuellen Mandanten dazuhängen
				$ra['elter']=$GLOBALS['defelt'];
		}

		// Rollback-Marke setzen
		db_rollback('start');

		switch($modus)
		{
			case 1:	// Neuanlage
				$nid=id_max($tabname, $sn);
				$tabid=$nid;

				$ra['*'.$sn]=(int)$nid;

				if($sv)
					$GLOBALS['elter'][$tabname][$sv]=$tabid;

				$sql_f='';
				$sql_v='';
				foreach($ra as $feldname => $f)
				{
					if(substr((string)$feldname, 0, 1) == '*')
						$feldname=substr((string)$feldname, 1);
					if($sql_f)
					{
						$sql_f .= ", ";
						$sql_v .= ", ";
					}
					$sql_f .= $feldname;
					if((gettype($f) == 'string') && ($f != 'NULL'))
						$f="'".$f."'";
					$sql_v .= $f;
				}
				$sql="INSERT INTO ".$tabname." (".$sql_f.") VALUES (".$sql_v.")";
//fxDebug($sql, '$sql');
				$serr=db_x($sql);

				// Zusatzdaten gleich mitanlegen bei Int. und Ext. Mitarbeiter
				if(!$serr && ($tabname == 'personen') && (($ra['personentyp'] == 275) || ($ra['personentyp'] == 840)))
				{
					// EK / VK / Arbeitstage
					$madaten_id=id_max('madaten', 'madaten_id');
					$maed=substr((string)fxNow(), 0, 6).'01000000';
					$sql  = "INSERT INTO madaten";
					$sql .= " (mandanten_id, firm_adress, personen_id, madaten_id, geburtsname, geburtsdatum, mobilbereich, schwerbehindert, maeintrittsdatum, maaustrittsdatum, blz, kontonummer, iban, bic, wochenarbeitstd, arbeitstage, soll_beginn, zeiterfassung, gleitzeit, montag, dienstag, mittwoch, donnerstag, freitag, samstag, sonntag, maxstdwoche, maxstdtag, zeitraumvon, zeitraumbis, pause)";
					$sql .= " VALUES";
					$sql .= " (".$GLOBALS['fxpglobals']['client'].", 0, ".$nid.", ".$madaten_id.", NULL, NULL, 0, 0, '".$maed."', NULL, NULL, NULL, NULL, NULL, 40, 5, '00000000080000', 1, 1, 1, 1, 1, 1, 1, 0, 0, NULL, NULL, NULL, NULL, 3600)";
//fxDebug($sql, '$sql');
					$serr=db_x($sql);

					if(!$serr)
					{
						// Artikelsätze (EK/VK/Urlaubsanspruch)
						if($ra['personentyp'] == 275)
							$ua=24.00;
						else
							$ua=0.00;
						$ru=0.00;

						$artikel_id=id_max('artikel', 'artikel_id');
						$sql  = "INSERT INTO artikel";
						$sql .= " (mandanten_id, artikel_id, personen_id, sachmittel_id, ek, vk, mwst_satz, soll_beginn, soll_ende, urlaubsanspruch, resturlaub, archiv)";
						$sql .= " VALUES";
						$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$artikel_id.", ".$nid.", NULL, 0.00, 0.00, 0.00, '".$maed."', NULL, ".$ua.", NULL, 0)";
//fxDebug($sql, '$sql');
						$serr=db_x($sql);

						if(!$serr)
						{
							$maej=substr((string)$maed, 0, 4);
							$artikel_id=id_max('artikel', 'artikel_id');
							$sql  = "INSERT INTO artikel";
							$sql .= " (mandanten_id, artikel_id, personen_id, sachmittel_id, ek, vk, mwst_satz, soll_beginn, soll_ende, urlaubsanspruch, resturlaub, archiv)";
							$sql .= " VALUES";
							$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$artikel_id.", ".$nid.", -1039, 0.00, 0.00, 0.00, '".$maej."0101000000', '".$maej."1231240000', ".$ua.", ".$ru.", 0)";
//fxDebug($sql, '$sql');
							$serr=db_x($sql);
						}
					}

					if(!$serr)
					{
						// Start-Übertrag
						$Zeit_ID=id_max('zeitdaten', 'zeit_id');
						$suj=(int)substr((string)$maed, 0, 4);
						$sum=(int)substr((string)$maed, 4, 2) - 1;
						if(!$sum)
						{
							$sum=12;
							$suj--;
						}
						$ndatum=$suj.substr('00'.$sum, -2).'00000000';
						$zd=array(
							'mandanten_id'			=> $GLOBALS['fxpglobals']['client'],
							'zuordnungs_mid'		=> $GLOBALS['fxpglobals']['client'],
							'personen_id'			=> $nid,
							'zeit_id'				=> $Zeit_ID,
							'projekt_id'			=> 0,
							'mutterprojekt_id'		=> 0,
							'rechnungs_id'			=> 0,
							'positions_id'			=> 0,
							'zkunde'				=> 0,

							'soll_beginn'			=> 'NULL',
							'soll_ende'				=> 'NULL',
							'ist_beginn'			=> "'$ndatum'",
							'ist_ende'				=> "'$ndatum'",
							'kw'					=> 0,
							'pause'					=> 0,
							'zeitaufwand'			=> 0,

							'kosten_ek'				=> 0,
							'kosten_vk'				=> 0,
							'kosten_mvk'			=> 0,
							'mwst_satz'				=> 0,

							'rk_ek'					=> 'NULL',
							'rk_vk'					=> 'NULL',
							'rk_ort1'				=> 'NULL',
							'rk_ort2'				=> 'NULL',
							'rk_km'					=> 'NULL',
							'rk_anznacht'			=> 'NULL',
							'rk_kategorie'			=> 'NULL',

							'nicht_abrechenbar'		=> 'NULL',
							'rk_nicht_abrechenbar'	=> 'NULL',
							'soll_verzoeg'			=> 'NULL',
							'ist_verzoeg'			=> 'NULL',
							'zeiteinheit'			=> 'NULL',
							'verz_grund'			=> 'NULL',

							'zeitart'				=> 105,
							'zeitgruppe'			=> 'NULL',
							'freigabe'				=> 0,
							'offline_zd'			=> 'NULL'
						);
						$zk='';
						$zv='';
						foreach($zd as $key => $value)
						{
							$zk .= ', '.$key;
							$zv .= ', '.$value;
						}
						$sql="INSERT INTO zeitdaten (".substr((string)$zk, 2).") VALUES (".substr((string)$zv, 2).")";
//fxDebug($sql, '$sql');
						$serr=db_x($sql);
					}
				}
			break;

			case 2:	// Ändern
				$tabid=$tid;

				// Alte Werte ermitteln
				$sql="SELECT * FROM ".$tabname." WHERE ".$sn."=".$tabid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$ov=db_values($sql);
				if(fxIsArray($ov))
				{
//fxDebug($ov, $sql);
					$old=' $old=array(';
					foreach($ra as $feldname => $f)
					{
						if(substr((string)$feldname, 0, 1) == '*')
							$feldname=substr((string)$feldname, 1);
						$fv=$ov[$feldname];
						if(!strlen((string)$fv))
							$fv='NULL';
						if(gettype($f) == 'string')
							$fv='"'.$fv.'"';
						$old .= '"'.$feldname.'"=>'.$fv.', ';
					}
					$old=substr((string)$old, 0, -2).');';
				}

				$sql='';
				foreach($ra as $feldname => $f)
				{
					if(substr((string)$feldname, 0, 1) == '*')
						$feldname=substr((string)$feldname, 1);
					if($sql)
						$sql .= ", ";
					if((gettype($f) == 'string') && ($f != 'NULL'))
						$f="'$f'";
					if($feldname == $sn)
						$f=$tabid;
					$sql .= $feldname."=".$f;
				}
				$sql="UPDATE ".$tabname." SET ".$sql." WHERE ".$sn."=".$tabid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
//fxDebug($sql, '$sql');
				$serr=db_x($sql);
			break;
		}
//$serr=true;

		if($serr)
		{
			$status=891+$modus;
			// SQL´s rückgängig
			db_rollback('rollback');
		}
		else
		{
			$status=887+$modus;
			// SQL´s ausführen
			db_rollback('ok');
		}
	}
	else if($tid)
		$tabid=$tid;

	if(($tabname == 'personen') && $tabid)
		$GLOBALS['epid']=$tabid;

	$args='$imp=array(';
	foreach($ra as $feldname => $f)
	{
		if(substr((string)$feldname, 0, 1) == '*')
			$feldname=substr((string)$feldname, 1);
		if(gettype($f) == 'string')
			$f='"'.str_replace('"', '_+_', $f).'"';
		$args .= '"'.$feldname.'"=>'.$f.', ';
	}
	$args=substr((string)$args, 0, -2).');'.$old;

	$sql  = "INSERT INTO import";
	$sql .= " (mandanten_id, id, elter, tabname, idintabellen, dateiname, dateigroesse, argumente, import_status, angelegt_von, anlage_dtm)";
	$sql .= " VALUES";
	$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$id.", ".$elter.", '".$tabname."', ".(int)$tabid.", '".$fname."', ".$size.", '".convert_string($args, 'todb')."', ".$status.", ".$apid.", '".$adtm."')";
//fxDebug($sql, '$sql');
	$ierr=db_x($sql);
	if($ierr || $serr)
		$err=true;

	return $err;
}

/**
 * Alle vorhandenen Nachschlagewerte ermitteln
 *
 * @param various $felder - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function import_getLookupValues($felder)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$nw=array();
	if(fxIsArray($felder))
	{
		foreach($felder as $feldname => $f)
		{
			if(($f['typ'] === 5) && ($f['def'] !== false))
			{
				if(!isset($nw[$feldname]))
					$nw[$feldname]=array();
				$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=".$f['id']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
				$fnw=db_values($sql,'*');
				if(fxIsArray($fnw))
				{
					foreach($fnw as $w)
						$nw[$feldname][strtolower((string)$w['tabwert'])]=$w['wertetabid'];
				}
				else
					$nw[$feldname]['###']=true;
			}
		}
	}

	return $nw;
}
?>