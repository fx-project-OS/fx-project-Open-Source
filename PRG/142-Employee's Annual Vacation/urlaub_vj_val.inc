<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : urlaub_vj_val.inc                                            //
// Version     : 21.1                                                         //
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
 * @version 21.1
 */

if(!$err && $disp_list && $_POST['Button_Multi'])
{
//fxDebug('_POST');

	// Aktuellen Schlüssel ermitteln
	reset($_POST['Personen_ID']);
	$mc=key($_POST['Personen_ID']);

	// Feld zum Speichern in welchen Feldern ein Fehler auftrat bzw. Erfolgsarray
	$uaerr=array();
	$uuerr=array();
	$plist=array();

	// Literale
	$ua_lit=$GLOBALS['fx_mfields']['125'][1]['literal_20'];
	$uu_lit=$GLOBALS['fx_mfields']['125'][2]['literal_20'];

	// Liste durchgehen
	$val_maskennummer=125;
	if(fxIsArray($_POST['Personen_ID'][$mc]))
	{
		foreach($_POST['Personen_ID'][$mc] as $ln => $pid)
		{
			$pid=(int)$pid;
			if(!$pid)
				continue;

			// Einträge vorhanden?
			$ua=trim($_POST['Urlaubsanspruch'][$mc][$ln]);
			$uu=trim($_POST['Urlaub_vj'][$mc][$ln]);
			if(!strlen($ua) && !strlen($uu))
				continue;

			$val_zeilennummer=$ln;

			// Einträge validieren
			$oua=get_double($_POST['nv_zahl1'][$mc][$ln]);
			if(strlen($ua))
			{
				$ua=validate_entry($ua, $verr, 'dezimal');
				if(!$verr)
				{
					$ua=get_double($ua);
					if(($ua < 0.00) || ($ua > 364.00))
						$verr=true;
				}
				if($verr)
				{
					// 1889: Bitte geben Sie für [$var1] in das Feld $var2 eine gültige Dezimalzahl zwischen $var3 und $var4 ein!
					error_msg(1889, array('Urlaubsanspruch'), '<b>'.$_POST['Pname'][$mc][$ln].'</b>', '<b>'.$ua_lit.'</b>', '<b>'.get_entry(0.0, 'dezimal').'</b>', '<b>'.get_entry(364.0, 'dezimal').'</b>');
					$uaerr[$pid][$ln]=true;
					$err=true;
				}
				else
					$_POST['Urlaubsanspruch'][$mc][$ln]=get_entry($ua, 'dezimal');
			}
			else
			{
				$ua=$oua;
				if(!$ua)
					$ua=0.00;
			}

			$ouu=get_double($_POST['nv_zahl2'][$mc][$ln]);
			if(strlen($uu))
			{
				$uu=validate_entry($uu, $verr, 'dezimal');
				if($verr)
				{
					// 1890: Bitte geben Sie für [$var1] in das Feld $var2 eine gültige Dezimalzahl ein!
					error_msg(1890, array('Urlaub_vj'), '<b>'.$_POST['Pname'][$mc][$ln].'</b>', '<b>'.$uu_lit.'</b>');
					$uuerr[$pid][$ln]=true;
					$err=true;
				}
				else
				{
					$uu=get_double($uu);
					$_POST['Urlaub_vj'][$mc][$ln]=get_entry($uu, 'dezimal');
				}
			}
			else
			{
				$uu=$ouu;
				if(!$uu)
					$uu=0.00;
			}

			// Einträge in Liste für DB
			if(!$uaerr[$ln] && !$uuerr[$ln] && (($ua != $oua) || ($uu != $ouu)))
				$plist[$pid][]=array('name'=>$_POST['Pname'][$mc][$ln], 'jahr'=>(int)$_POST['nv_ganzzahl1'][$mc][$ln], 'artikel_id'=>(int)$_POST['Artikel_ID'][$mc][$ln], 'oua'=>$oua, 'ua'=>$ua, 'ouu'=>$ouu, 'uu'=>$uu);
		}
	}
//$err=true;
}
?>