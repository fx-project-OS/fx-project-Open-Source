<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : person_ergebnis_pre.inc                                      //
// Version     : 21.2                                                         //
// Begin       : 2020-12-22                                                   //
// Last Change : 2020-12-22                                                   //
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
 * PF 82: Person Search
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$mc=$GLOBALS['_maskcounter'];
$sql_search='';

// Abfrage auf den Namen
if($Pname)
	$sql_search .= " AND ".db_like('p.pname', '%'.$Pname.'%');

// Abfrage auf den Vornamen
if($Vorname)
	$sql_search .= " AND ".db_like('p.vorname', '%'.$Vorname.'%');

// Abfrage auf Personentyp
if($PersonenTyp)
	$sql_search .= " AND p.personentyp=".$PersonenTyp;

// Abfrage auf die Abteilung
if($Abt_ID)
	$sql_search .= " AND p.abt_id=".$Abt_ID;

// Abrage auf die Personen-Position
if($Pposition)
	$sql_search .= " AND p.pposition=".$Pposition;

// Nur Personen suchen, auf die man Rechte hat
$sql_search .= " AND p.personen_id IN (".get_persons().")";

// evtl. Anzeige der Komm-Daten
if(!$Kontaktmoeglichkeit)
{
	$KommunikationsTyp_nv=true;
	$Kommunikation_txt_nv=true;
}

// Filter
$flt=filter_person('and', true, false, 'Personen_ID');
$maske221_flt=$mskfilter;

$sql="SELECT p.pname, p.vorname, p.inaktiv, p.geschlecht, p.pposition, p.personentyp, p.personen_id, p.abt_id, p.elter, a.strasse, a.ort, a.plz, a.landkz";
if($Kontaktmoeglichkeit)
	$sql .= ", k.kommunikationstyp, k.kommunikation_txt";
$sql .= " FROM personen p LEFT JOIN adressen a ON (p.personen_id=a.personen_id AND p.mandanten_id=a.mandanten_id)";
if($Kontaktmoeglichkeit)
	$sql .= " LEFT JOIN kommunikation k ON (a.personen_id=k.personen_id AND a.mandanten_id=k.mandanten_id)";
$sql .= " WHERE p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
$sql .= $sql_search.$flt;
$sql .= " ORDER BY p.personentyp, ".substr(db_orderby('p.pname', 'p.vorname'), 9);
//echo('<B>SQL:</B><BR>'.$sql.'<HR>');

// Listenwerte aus DB ermitteln
$GLOBALS['fxptdata']['lvalues'][$mc]=array();
$tmp=db_values($sql, '*');
if(fxIsArray($tmp))
{
	$inakt=' <i class="lightergrey">{'.get_text(1391, '20').'}</i>';	// {inaktiv}
	$pta=array();

	// NoWrap für folgende Felder
	$Pname_nw				= true;
	$Vorname_nw				= true;
	$PersonenTyp_nw			= true;
	$Abt_ID_nw				= true;
	$Pposition_nw			= true;
	$Strasse_nw				= true;
	$PLZ_nw					= true;
	$Ort_nw					= true;
	$LandKZ_nw				= true;
	$KommunikationsTyp_nw	= true;
	$Kommunikation_txt_nw	= true;

	// Mask fields
	$fa=fxf_getMaskFields(221,false,array('Dummy'=>0));
//fxDebug($fa,'$fa', 0);

	// Abteilungs-Namen holen
	$abtname=auswahl_Abt_ID(1);
	$abtname[0]=db_value("SELECT pname FROM personen WHERE personentyp=274 AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

	// Array das für die jeweilige Personengruppe auf die passende Transaktion verweisst ("Personen kompakt" und "alt")
	$to_trans=array(274 => array(166, 15), 275 => array(167, 50), 276 => array(169, 51), 620 => array(170, 54), 840 => array(168, 52), 841 => array(172, 116), 1372 => array(171, 94), 2090 => array(164, 65), 2091 => array(173, 129), 2092 => array(174, 130));

	$l=0;
	$r=0;
	foreach($tmp as $lw)
	{
		// New person typ
		$p=(int)$lw['personentyp'];
		if($r && ($p != $r))
		{
			fxf_maskColumnBorder($fa, 1, -1, $mc, $l, true);
			$l++;
		}
		$r=$p;

		$GLOBALS['fxptdata']['lvalues'][$mc][$l]=$lw;

		// Name
		if($lw['inaktiv'])
			$nam='<font class="grey">'.$lw['pname'].'</font>'.$inakt;
		else
			$nam=$lw['pname'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['pname']='~'.$nam;

		// Vorname
		if($lw['inaktiv'])
			$nam='<font class="grey">'.$lw['vorname'].'</font>';
		else
			$nam=$lw['vorname'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['vorname']='~'.$nam;

		// Pers.-Typ
		if(!isset($pta[$lw['personentyp']]))
			$pta[$lw['personentyp']]=meldung($lw['personentyp']);
		// Anhängen des Namen des Elters des Kundenansprechpartners
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['personentyp']='~'.$pta[$lw['personentyp']];
		if($lw['personentyp'] == 620)
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['personentyp'] .= ' <font class="grey">['.db_value("SELECT pname FROM personen WHERE personen_id=".$lw['elter']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']).']</font>';

		// Abteilungsname
		if(!$lw['abt_id'])
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['abt_id']=0;
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['abt_id']='~'.$abtname[$GLOBALS['fxptdata']['lvalues'][$mc][$l]['abt_id']];

		// Stellenpos.
		if(!$lw['pposition'])
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['pposition']='';

		// E-Mails als Link
		if(strpos($lw['kommunikation_txt'], '@'))
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['kommunikation_txt']='~<A HREF="mailto:'.$lw['kommunikation_txt'].'">'.$lw['kommunikation_txt'].'</A>';

		// Ist auf die Personkompakt-Transaktion ein Recht, Link setzen ...
		if(tr_rechte($to_trans[$lw['personentyp']][0]))
			$destination=$to_trans[$lw['personentyp']][0];
		// ... ist auf die Personkompakt-Transaktion kein Recht, dann auf alte Transaktion ...
		else if(tr_rechte($to_trans[$lw['personentyp']][1]))
			$destination=$to_trans[$lw['personentyp']][1];
		// ... ist auf die alte Transaktion auch kein Recht => Link ausblenden
		else
		{
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['dummy']='~';
			continue;
		}
		$bildchen=person_pic($lw['personen_id'], 20, $lw['geschlecht'], $GLOBALS['fxpglobals']['client'], 0);
		$op=1.0;
		if(substr($bildchen,0,strlen($GLOBALS['gfxpath'])) == $GLOBALS['gfxpath'])
			$op=0.25;
		if(!isset($GLOBALS['ltt_'.$destination]))
			$GLOBALS['ltt_'.$destination]='<b>'.meldung(1697,false).'</b>: '.trPath($destination);	// Sprung zu
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['dummy']='~'.fieldLinkImage('lnk'.$l, $bildchen, '?pid='.$lw['personen_id'].'&ptyp='.$lw['personentyp'], $GLOBALS['ltt_'.$destination], $destination, 'border:1px solid #aaaaaa;border-radius:4px;background:#ffffff;padding:1px;opacity:'.$op.';', '', 20, 20);

		$l++;
	}

	// Last line with Top Border?
	if($l)
	{
		fxf_maskColumnBorder($fa, 1, -1, $mc, $l, true);
		$l++;
	}
}
?>