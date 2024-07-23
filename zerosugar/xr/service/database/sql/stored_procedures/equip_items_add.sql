DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `equip_items_add`(
    IN character_id BIGINT,
    IN json_str JSON
)
BEGIN
    DECLARE EXIT HANDLER FOR SQLEXCEPTION 
    BEGIN
          ROLLBACK;
    END;

    START TRANSACTION;

    INSERT INTO items
        (iid, cid, data_id, quantity, attack, defence, str, dex, intell)
    SELECT 
        j.item_id, character_id, j.item_data_id, j.quantity, j.attack, j.defence, j.str, j.dex, j.intell
    FROM
        JSON_TABLE(
            json_str,
            '$[*].item' COLUMNS (
                item_id BIGINT PATH '$.itemId',
                item_data_id INT PATH '$.itemDataId',
                quantity INT PATH '$.quantity',
                attack INT PATH '$.attack',
                defence INT PATH '$.defence',
                str INT PATH '$.str',
                dex INT PATH '$.dex',
                intell INT PATH '$.intell'
            )
        ) as j;
    
    INSERT INTO characters_equipment
        (cid, iid, equip_position)
    SELECT
        character_id, j.item_id, j.equip_position
    FROM
        JSON_TABLE(
            json_str,
            '$[*]' COLUMNS (
                equip_position INT PATH '$.equipPosition',
                item_id BIGINT PATH '$.item.itemId'
            )
        )
        AS j;

    COMMIT;
END$$
DELIMITER ;
