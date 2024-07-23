DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `items_update`(
    IN jsonArray JSON
)
BEGIN
    DECLARE i INT DEFAULT 0;
    DECLARE size INT;
    DECLARE type INT;
    DECLARE element JSON;
    DECLARE data JSON;

    DECLARE CONTINUE HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
    END;

    SET size = JSON_LENGTH(jsonArray);

    START TRANSACTION;

    WHILE i < size DO 
        SET element = JSON_EXTRACT(jsonArray, CONCAT('$[', i, ']'));
        
        SET type = JSON_EXTRACT(element, '$.type');
        SET data = JSON_EXTRACT(element, '$.data');
                
        CASE type
            WHEN 1 THEN # equip
                INSERT INTO characters_equipment
                    (cid, iid, equip_position)
                VALUES (
                    JSON_EXTRACT(data, '$.characterId'),
                    JSON_EXTRACT(data, '$.itemId'),
                    JSON_EXTRACT(data, '$.equipPosition')
                );
                
                UPDATE items
                SET slot = null
                WHERE iid =  JSON_EXTRACT(data, '$.itemId');
                
            WHEN 2 THEN # unequip
                DELETE FROM characters_equipment
                WHERE iid = JSON_EXTRACT(data, '$.itemId');
                
                UPDATE items
                SET slot = JSON_EXTRACT(data, '$.inventorySlot')
                WHERE iid = JSON_EXTRACT(data, '$.itemId');
                
            WHEN 3 THEN # shift
                UPDATE items
                SET slot = JSON_EXTRACT(data, '$.inventorySlot')
                WHERE iid = JSON_EXTRACT(data, '$.itemId');
                
            WHEN 4 THEN # add
                INSERT INTO items
                    (iid, cid, data_id, quantity, slot)
                VALUES (
                    JSON_EXTRACT(data, '$.itemId'),
                    JSON_EXTRACT(data, '$.characterId'),
                    JSON_EXTRACT(data, '$.itemDataId'),
                    JSON_EXTRACT(data, '$.quantity'),
                    JSON_EXTRACT(data, '$.slot')
                );

            WHEN 5 THEN # remove, TODO: soft delete
                DELETE FROM items
                WHERE iid = JSON_EXTRACT(data, '$.itemId');
                
            WHEN 6 THEN # change item quantity
                UPDATE items
                SET quantity = JSON_EXTRACT(data, '$.quantity')
                WHERE iid = JSON_EXTRACT(data, '$.itemId');

        END CASE;

        SET i = i + 1;

    END WHILE;

    COMMIT;
END$$
DELIMITER ;
