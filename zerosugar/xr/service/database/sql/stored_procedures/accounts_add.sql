DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `accounts_add`(
    account VARCHAR(20),
    password VARCHAR(64)
)
BEGIN
    INSERT INTO accounts 
        (account, password)
    VALUES 
        (account, password);
END$$
DELIMITER ;
