DELETE FROM `command` WHERE `name` IN ('classtrial');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('classtrial', 0, 'Syntax: .classtrial \nApplies a class trial to your character.');
