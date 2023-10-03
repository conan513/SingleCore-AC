SET
@string = 40050;

DELETE FROM `acore_string` WHERE `entry` BETWEEN @string AND @string+1;
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(@string, 'This server is running the Class Trial module.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(@string+1, 'Class trials are currently disabled.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
