<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : userprefs_db.inc                                             //
// Version     : 24.1                                                         //
// Begin       : 2020-09-22                                                   //
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
 * PF 25: User Settings - Database file for saving the adjusted user settings
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

//fxDebug('_POST');
if(fxIsArray($_POST) && $_POST['Button_Submit'] && fxIsArray($_POST['newval']) && fxIsArray($_POST['oldval']) && fxIsArray($_POST['vr']))
{
	$chg=0;
	$del=0;

	foreach($_POST['newval'] as $id => $nv)
	{
		$typ=trim((string)$_POST['typ'][$id]);
		$vergaberecht=(int)$_POST['vr'][$id];
		$ov=trim((string)$_POST['oldval'][$id]);

		// Was a value unequal to the current value submitted?
		$nv=trim((string)$nv);
		if($id == 35)
			$ra=getSettingMaxCL($nv);
		$cnv=$nv;
		// ...Adjust value according to its type
		if($typ == 'km')
			$cnv=get_entry($cnv, 'km_float');
		else if($typ == 'float')
		{
			$ov=(float)$ov;
			$cnv=get_double($cnv);
		}
		else if($typ == 'integer')
		{
			$ov=(int)$ov;
			$cnv=(int)($cnv);
		}
		// Should the value be deleted - this is accomplished by negating the value?
		$aid=$id;
		if((fxIsArray($_POST['Loeschen']) && $_POST['Loeschen'][$id]) || (fxIsArray($_POST['loeschen']) && $_POST['loeschen'][$id]))
			$aid=-$id;
//echo('$aid='.$aid.' ($id='.$id.'), $typ='.$typ.', $vergaberecht='.$vergaberecht.', $ov="'.$ov.'", $nv="'.$nv.'", $cnv="'.$cnv.'"<br />');
		if(!$aid || (($aid > 0) && ($cnv === $ov)))
			continue;

		// Exists an old setting?
		if($vergaberecht < 3)									// Sysadmin, Admin1 or Admin
			$setval_user=1;
		else													// user
			$setval_user=$GLOBALS['fxpglobals']['user'];

		if(!$vergaberecht)										// Sysadmin
			$setval_client=0;
		else if($vergaberecht == 1)								// Admin1
			$setval_client=1;
		else													// Admin or Benutzer
			$setval_client=$GLOBALS['fxpglobals']['client'];

		// Get sql and execute it
		$sql=setSetting(false, $aid, $cnv, $setval_user, $setval_client);
		$err=db_x($sql);
fxDebug($err, '$aid='.$aid.', $cnv='.$cnv.', $setval_user='.$setval_user.', $setval_client='.$setval_client.'<br />$sql='.$sql.'<br />$err', 2);
		if($err)
		{
			if($aid < 0)
				error_msg(1523, false, $id);	// An error occurred while deleting the data record "$var1"!
			else
				error_msg(1522, false, $id);	// An error occurred while changing the data record "$var1"!
			break;
		}
		else if($aid < 0)
			$del++;
		else
			$chg++;
	}

	// Success message
	if(!$err)
	{
		if(!$chg && !$del)
			error_msg(160, false);				// No changes were made.
		else
		{
			if($del == 1)
				error_msg(119, false);			// One data record was successfully deleted.
			else if($del)
				error_msg(116, false, $del);	// $var1 data records were successfully deleted.

			if($chg == 1)
				error_msg(118, false);			// One data record was successfully changed.
			else if($chg)
				error_msg(115, false, $chg);	// $var1 data records were successfully changed.

			// Update global user settings
			$GLOBALS['fxpglobals']['settings']=benutzereinstellungen($GLOBALS['fxpglobals']['user']);
		}
	}
//$err=true;
}
?>