<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_adr_val.inc                                 //
// Version     : 21.1                                                         //
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
 * PF 166: Client etc. - Address validation (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

// Variable preparations
$err_adr=false;

$akey=-1;
if(fxIsArray($_POST) && fxIsArray($_POST['Adressen_ID']))
{
	reset($_POST['Adressen_ID']);
	$akey=key($_POST['Adressen_ID']);
}

// Cycle through all addresses
if(($akey >= 0) && fxIsArray($_POST['Adressen_ID'][$akey]))
{
	$mand_texts=array('Strasse', 'PLZ', 'Ort');
	$mand_selects=array('AdressTyp', 'LandKZ');
	foreach($_POST['Adressen_ID'][$akey] as $acnt => $aval)
	{
		$val_zeilennummer=$acnt;

		// Empty line?
		if(!strlen($aval))
			continue;

		// Empty dataset possible for creation
		if($aval < 0)
		{
			if(!$_POST['AdressTyp'][$akey][$acnt] && !$_POST['LandKZ'][$akey][$acnt] && !strlen($_POST['Staat'][$akey][$acnt]) && !strlen($_POST['Strasse'][$akey][$acnt]) && !strlen($_POST['PLZ'][$akey][$acnt]) && !strlen($_POST['Ort'][$akey][$acnt]))
				continue;
		}

		// Mandatory text fields
		foreach($mand_texts as $fname)
		{
			if(!strlen($_POST[$fname][$akey][$acnt]))
			{
				// The field "$var1" cannot be empty. Please enter a text!
				error_msg(749, false, $fname);
				$err_adr=true;
			}
		}
		// Mandantory select fields
		foreach($mand_selects as $fname)
		{
			if(!$_POST[$fname][$akey][$acnt])
			{
				// The field "$var1" must be selected!
				error_msg(750, false, $fname);
				$err_adr=true;
			}
		}
	}
}

if($err_adr)
	$err=true;
?>