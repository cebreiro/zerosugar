USE `xr_project`;
DROP procedure IF EXISTS `account_get`;

DELIMITER $$
USE `xr_project`$$
CREATE PROCEDURE `account_get` (
	account varchar(20)
)
BEGIN
	SELECT * from accounts WHERE accounts.account = account;
END$$

DELIMITER ;
