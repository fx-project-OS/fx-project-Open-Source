<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : dbt_correct.inc                                              //
// Version     : 21.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2020-07-22                                                   //
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
 * Adjust DB tables and fields to newer versions
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if($from_dbt_update === true)
{
	$correct_err=false;

	// Feldnamen-Arrays neu erstellen für Memo-, Char-, Float- und Integer-Felder
	fxf_dbFields(true);

	if(!strlen($old_version))
		$old_version = $GLOBALS['fxpglobals']['dbparam']['versionscode_db'];

	// Fehlerhafte Vorgangsnummern korrigieren
	db_slog('*Correct faulty process numbers');
	if($correct_err)
		db_slog('SKIPPED, because of a previous error!');
	else
	{
		$erc = 0;
		$cor = 0;
		$sql = "SELECT projekt_id, vorgangsnummer FROM projekte WHERE projekt_id>0 ORDER BY vorgangsnummer";
		$vna = db_values($sql, '*');
		if(fxIsArray($vna))
		{
			foreach($vna as $v)
			{
				$id = (int)$v['projekt_id'];
				$ov = trim($v['vorgangsnummer']);

				$nv = '';
				$ve = explode('.', $ov);
				foreach($ve as $vl)
				{
					if(strlen($nv))
						$nv .= '.';
					$nv .= substr('0000'.(int)$vl, -4);
				}

				if($ov != $nv)
				{
					$erc++;

					$sql = "UPDATE projekte SET vorgangsnummer='".$nv."' WHERE projekt_id=".$id;
					$success = db_query($sql);
					if($success)
						$cor++;
				}
			}
		}

		if(!$erc)
			db_slog('OK. No process number had to be corrected.');
		else
			db_slog($cor.' of '.$erc.' faulty process numbers have been corrected!');
	}

	// Abrechnungsbeträge von NULL auf 0 ändern, sodass Datenbankadditionsfunktionen arbeiten (generell)
	db_slog('*Update NULL invoice budgets');
	if($correct_err)
		db_slog('SKIPPED, because of a previous error!');
	else
	{
		db_rollback('start');
		$cerr = false;

		$bs_ending_array = array('_ext', '_ext_off');
		$bs_invoicefield_array = array('fest', 'std', 'reise', 'mat');
		foreach($bs_ending_array as $bs_ending)
		{
			if($cerr)
				break;

			foreach($bs_invoicefield_array as $bs_invoicefield)
			{
				if($cerr)
					break;

				$bs_dbcolumn = 'rechnung_'.$bs_invoicefield.$bs_ending;

				$sql = "UPDATE budget_summe SET ".$bs_dbcolumn."=0 WHERE ".$bs_dbcolumn." IS NULL";
				$success = db_query($sql);
				if(!$success)
					$cerr = true;
			}
		}

		if($cerr)
		{
			db_rollback('rollback');
			$correct_err = true;
			db_slog('ERROR! Correction script failed.');
		}
		else
		{
			db_rollback('ok');
			db_slog('OK, all done.');
		}
	}
}
?>