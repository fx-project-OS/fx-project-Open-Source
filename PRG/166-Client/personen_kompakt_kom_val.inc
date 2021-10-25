<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_kom_val.inc                                 //
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
 * PF 166: Client etc. - Communication validation (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Variable preparations
$err_com=false;

$akey=-1;
if(fxIsArray($_POST) && fxIsArray($_POST['Kommunikation_ID']))
{
	reset($_POST['Kommunikation_ID']);
	$akey=key($_POST['Kommunikation_ID']);
}

// Cycle through all communications
if(($akey >= 0) && fxIsArray($_POST['Kommunikation_ID'][$akey]))
{
	$mand_texts=array('Kommunikation_txt');
	$mand_selects=array('KommunikationsTyp');

	foreach($_POST['Kommunikation_ID'][$akey] as $ccnt => $cval)
	{
		$val_zeilennummer=$ccnt;

		// Empty line?
		if(!strlen($cval))
			continue;

		// Empty dataset possible for creation
		if($cval < 0)
		{
			if(!$_POST['KommunikationsTyp'][$akey][$ccnt] && !strlen($_POST['Kommunikation_txt'][$akey][$ccnt]) && !strlen($_POST['Anmerkung'][$akey][$ccnt]))
				continue;
		}

		// Mandatory text fields
		foreach($mand_texts as $fname)
		{
			if(!strlen($_POST[$fname][$akey][$ccnt]))
			{
				// The field "$var1" cannot be empty. Please enter a text!
				error_msg(749, false, $fname);
				$err_com=true;
			}
			else if($_POST['KommunikationsTyp'][$akey][$ccnt] == 2366)	// Defaul Email Address
			{
				$_POST[$fname][$akey][$ccnt]=validate_entry($_POST[$fname][$akey][$ccnt], $verr, 'email');
				if($verr)
				{
					$err_com=true;
					error_msg($verr, array($fname));
				}
			}
		}
		// Mandantory select fields
		foreach($mand_selects as $fname)
		{
			if(!$_POST[$fname][$akey][$ccnt])
			{
				// The field "$var1" must be selected!
				error_msg(750, false, $fname);
				$err_com=true;
			}
		}
	}
}

if($err_com)
	$err=true;
?>