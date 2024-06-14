USE `xr_project`;
DROP procedure IF EXISTS `account_create`;

DELIMITER $$
USE `xr_project`$$
CREATE PROCEDURE `account_create`(
	account varchar(20),
    password varchar(64)
)
BEGIN
	INSERT INTO accounts 
    (account, password) VALUES 
    (account, password);
END$$

DELIMITER ;
