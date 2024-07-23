DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `accounts_get`(
    account VARCHAR(20)
)
BEGIN
    SELECT 
        * 
    FROM
        accounts 
    WHERE 
        accounts.account = account;
END$$
DELIMITER ;
