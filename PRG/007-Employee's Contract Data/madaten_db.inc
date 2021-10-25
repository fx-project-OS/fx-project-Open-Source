<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : madaten_db.inc                                               //
// Version     : 21.2                                                         //
// Begin       : 2020-10-20                                                   //
// Last Change : 2020-10-20                                                   //
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
 * PF 7: Employee's Contract Data
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$dta=explode('|', meldung(3345,false));	// Vertragsdaten|Kostendaten|Urlaubsdaten|Projektkostendaten|Projektauslastung
$ata=explode('|', meldung(3346,false));	// hinzugefügt|abgeändert|gelöscht|Hinzufügen|Abändern|Löschen
// ...Name
$name='?';
if(fxIsArray($prsarr))
{
	$name=$prsarr['pname'];
	if(strlen($prsarr['vorname']))
		$name .= ', '.$prsarr['vorname'];
}
fxDebug($name,'$name', 3);

// Vertragsdaten
// -----------------------------------------------------------------------------------------------------------------
$mok=0;
$mko=0;
$vch=false;	// wird gesetzt falls an den Vertragsdaten etwas verändert wurde
$dch=false;	// wird gesetzt falls am Ein- oder Austrittsdatum etwas verändert wurde
$kch=false;	// wird gesetzt falls am EK/VK etwas verändert wurde
$uch=false;	// wird gesetzt falls am Urlaub etwas verändert wurde
$hdat=array();
$aam='';
if(!$verr && !$kerr && fxIsArray($udbarr))
{
	// Historiendaten
	$hdat=getPersonData($Personen_ID);

	// madaten
	if(!$verr)
	{
		$sql="";
		foreach($udbarr as $c => $v)
		{
			if(($c != 'ek') && ($c != 'vk') && ($c != 'urlaubsanspruch') && ($c != 'resturlaub'))
			{
				if(strlen($sql))
					$sql .= ", ";
				$sql .= $c."=".$v;
			}
		}
		if(strlen($sql))
		{
			$sql="UPDATE madaten SET ".$sql." WHERE personen_id=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$mid=(int)$_POST['madaten_id'];
			if($mid > 0)
				$sql .= " AND madaten_id=".$mid;
			$verr=db_x($sql);
fxDebug($verr,$sql, 2);
			if($verr)
			{
				error_msg(3348,false, $ata[4],'<b class=red>'.$dta[0].'</b>','&quot;<b>'.$name.'</b>&quot;');	// Fehler beim $var1 (=Abändern) der $var2 (=Vertragsdaten) von $var3 (=Name)!
				$mko++;
			}
			else
			{
				$mok++;
				$vch=true;
				if(isset($udbarr['maeintrittsdatum']) || isset($udbarr['maaustrittsdatum']))
					$dch=true;
			}
		}
	}

	// artikel (EK + VK)
	if(!$verr && isset($udbarr['ek']))
	{
		// ...Bisherige Werte archivieren
		$oaid=(int)$_POST['artikel_id'];
		if($oaid > 0)
		{
			$sql="UPDATE artikel SET soll_ende='".$GLOBALS['datetime']."', archiv=1 WHERE artikel_id=".$oaid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$verr=db_x($sql);
fxDebug($verr,$sql, 2);
			if($verr)
			{
				error_msg(3348,false, $ata[4],'<b class=red>'.$dta[1].'</b>','&quot;<b>'.$name.'</b>&quot;');	// Fehler beim $var1 (=Abändern) der $var2 (=Kostendaten) von $var3 (=Name)!
				$mko++;
			}
			else
				$mok++;
		}

		// ...Neue Werte schreiben
		if(!$verr)
		{
			$naid=id_max('artikel', 'artikel_id');

			$sql  = "INSERT INTO artikel";
			$sql .= " (mandanten_id, artikel_id, personen_id, sachmittel_id, ek, vk, mwst_satz, soll_beginn, soll_ende, urlaubsanspruch, resturlaub, archiv) ";
			$sql .= "VALUES";
			$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$naid.", ".$Personen_ID.", NULL, ".$udbarr['ek'].", ".$udbarr['vk'].", 0, '".$GLOBALS['datetime']."', NULL, NULL, NULL, 0)";
			$verr=db_x($sql);
fxDebug($verr,$sql, 2);
			if($verr)
			{
				error_msg(3348,false, $ata[3],'<b class=red>'.$dta[1].'</b>','&quot;<b>'.$name.'</b>&quot;');	// Fehler beim $var1 (=Hinzufügen) der $var2 (=Kostendaten) von $var3 (=Name)!
				$mko++;
			}
			else
				$mok++;

			// Ist der Mitarbeiter, dessen Artikeldaten geändert wurden, einem Vorgang zugeordnet, so werden diese Datensätze mit der neuen Artikel-ID aktualisiert
			if(!$verr && ($oaid > 0))
			{
				$sql="UPDATE artikelzuordnung SET artikel_id=".$naid." WHERE artikel_id=".$oaid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$verr=db_x($sql);
fxDebug($verr,$sql, 2);
				if($verr)
				{
					error_msg(3348,false, $ata[4],'<b class=red>'.$dta[3].'</b>','&quot;<b>'.$name.'</b>&quot;');	// Fehler beim $var1 (=Abändern) der $var2 (=Projektkostendaten) von $var3 (=Name)!
					$mko++;
				}
				else
					$mok++;
			}
		}

		if(!$verr)
			$kch=true;
	}

	// artikel (Urlaubsanspruch + Resturlaub)
	if(!$verr && isset($udbarr['urlaubsanspruch']))
	{
		$year=(int)substr($GLOBALS['datetime'],0,4);
		$verr=reset_resturlaub($Personen_ID, $year);
		$uerr=4;
		if(!$verr)
		{
			$von=substr('0000'.$year, -4).'0101000000';
			$bis=substr('0000'.$year, -4).'1231240000';

			$uid=db_value("SELECT artikel_id FROM artikel WHERE personen_id=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND sachmittel_id=-1039 AND soll_beginn='".$von."'");
			if($uid)
				$sql="UPDATE artikel SET urlaubsanspruch=".$udbarr['urlaubsanspruch'].", resturlaub=".$udbarr['resturlaub'].", archiv=0 WHERE artikel_id=".$uid." AND personen_id=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			else
			{
				$uerr=3;
				$uid=id_max('artikel', 'artikel_id');

				$sql  = "INSERT INTO artikel";
				$sql .= " (mandanten_id, artikel_id, personen_id, sachmittel_id, ek, vk, mwst_satz, soll_beginn, soll_ende, urlaubsanspruch, resturlaub, archiv) ";
				$sql .= "VALUES";
				$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$uid.", ".$Personen_ID.", -1039, 0, 0, 0, '".$von."', '".$bis."', ".$udbarr['urlaubsanspruch'].", ".$udbarr['resturlaub'].", 0)";
			}
			$verr=db_x($sql);
fxDebug($verr,$sql, 2);
			if($verr)
				$mko++;
			else
				$mok++;
		}
		else
			$mko++;

		if(!$verr)
			$uch=true;
		else
			error_msg(3348,false, $ata[$uerr],'<b class=red>'.$dta[2].'</b>','&quot;<b>'.$name.'</b>&quot;');	// Fehler beim $var1 (=Hinzufügen/Abändern) der $var2 (=Urlaubsdaten) von $var3 (=Name)!
	}

	// Projektauslastungen neu berechnen und Abwesenheitstermine anpassen, falls sich wichtige Vertragsdaten verändert haben?
	if(!$verr)
	{
		$recalc=false;
		$vca=array('maeintrittsdatum', 'maaustrittsdatum', 'wochenarbeitstd', 'arbeitstage');
		foreach($vca as $vc)
		{
			if(isset($udbarr[$vc]))
				$recalc=true;
		}
		if($recalc)
		{
			$verr=update_auslastung($Personen_ID);
			if($verr)
			{
				error_msg(3348,false, $ata[4],'<b class=red>'.$dta[4].'</b>','&quot;<b>'.$name.'</b>&quot;');	// Fehler beim $var1 (=Abändern) der $var2 (=Projektauslastung) von $var3 (=Name)!
		   		$mko++;
		   	}
		   	else
		   	{
				$verr=update_appointments($Personen_ID);
				if($verr)
				{
					error_msg(3350,false, '<br /><br />'.$GLOBALS['_uama']['ko']);	// Fehler beim Anpassen der Abwesenheitstermine!$var1
			   		$mko++;
			   	}
			   	else
			   	{
			   		$aam=$GLOBALS['_uama']['ok'];
			   		$mok++;
			   	}
		   	}
		}
	}
}

if($verr || $kerr)
	$err=true;

// Keine Fehler bisher -> Erfolgsmeldungen
if(!$err)
{
	$sd='';
	if($mok)
	{
		if($vch)
			$sd .= ','.$dta[0];	// Vertragsdaten
		if($kch)
			$sd .= ','.$dta[1];	// Kostendaten
		if($uch)
			$sd .= ','.$dta[2];	// Urlaubsdaten
	}

	if(strlen($sd))
	{
		if(strlen($aam))
			error_msg(3349, false, '<br /><br />'.$aam);	// 3349: Die Abwesenheitstermine wurden erfolgreich angepasst.$var1

		$sd=substr($sd,1);
		$lcp=strrpos($sd,',');
		if($lcp !== false)
			$sd=substr($sd,0,$lcp).'</b> + <b class=green>'.substr($sd,$lcp+1);
		$sd=strtr($sd, array(','=>'</b>, <b class=green>'));
		error_msg(3347,false, '<b class=green>'.$sd.'</b>','&quot;<b>'.$name.'</b>&quot;',$ata[1]);	// Die $var1 (=Daten) von $var2 (=Name)wurden erfolgreich $var3 (=abgeändert).
	}
	else
		error_msg(160,false);	// Es wurden keine Änderungen durchgeführt.
}

//$verr=true; $kerr=true;
if($verr || $kerr)
	$err=true;

// Kein Fehler bisher, und es hat sich das Ein- oder Austrittsdatum verändert, dann evtl. E-Mails versenden
if(!$err && $dch)
{
	$mpa=getMailPersons(array(), 3065, $Personen_ID);	// Setting 3065: ...the entry/leave date or the status of an employee has been changed
	if(fxIsArray($mpa))
	{
		$ndat=getPersonData($Personen_ID);
		$sent=sendEmployeeDataMails($mpa, $Personen_ID, $ndat, $hdat);
		if(strlen($GLOBALS['_sent_success']) || strlen($GLOBALS['_sent_error']))
		{
			if(strlen($GLOBALS['_sent_error']))
			{
				$ec=substr_count($GLOBALS['_sent_error'], '<br />');
				if($ec == 1)
					error_msg(3069,false,':'.$GLOBALS['_sent_error']);			// 3069: The following person could not be informed by Email$var1
				else
					error_msg(3070,false,$ec,':'.$GLOBALS['_sent_error']);		// 3070: The following $var1 persons could not be informed by Email$var2
			}
			if(strlen($GLOBALS['_sent_success']))
			{
				$ec=substr_count($GLOBALS['_sent_success'], '<br />');
				if($ec == 1)
					error_msg(3071,false,':'.$GLOBALS['_sent_success']);		// 3071: The following person has been successfully informed by Email$var1
				else
					error_msg(3072,false,$ec,':'.$GLOBALS['_sent_success']);	// 3072: The following $var1 persons have been successfully informed by Email$var2
			}
		}
	}
}
?>