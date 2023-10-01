-- MySQL dump 10.13  Distrib 8.0.34, for Linux (x86_64)
--
-- Host: localhost    Database: acore_world
-- ------------------------------------------------------
-- Server version	8.0.34-0ubuntu0.22.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `auctionator_item_class`
--

DROP TABLE IF EXISTS `auctionator_item_class`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `auctionator_item_class` (
  `class` tinyint unsigned NOT NULL,
  `subclass` tinyint unsigned DEFAULT NULL,
  `name` varchar(100) COLLATE utf8mb4_general_ci NOT NULL,
  UNIQUE KEY `auctionator_item_class_class_IDX` (`class`,`subclass`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `auctionator_item_class`
--

LOCK TABLES `auctionator_item_class` WRITE;
/*!40000 ALTER TABLE `auctionator_item_class` DISABLE KEYS */;
INSERT INTO `auctionator_item_class` VALUES (0,NULL,'consumable'),(1,NULL,'container'),(2,NULL,'weapon'),(3,NULL,'gem'),(4,NULL,'armor'),(5,NULL,'reagent'),(6,NULL,'projectile'),(7,NULL,'trade goods'),(8,NULL,'generic'),(9,NULL,'recipe'),(10,NULL,'money'),(11,NULL,'quiver'),(12,NULL,'quest'),(13,NULL,'key'),(14,NULL,'permanent'),(15,NULL,'misc'),(16,NULL,'glyph'),(0,0,'consumable'),(0,1,'potion'),(0,2,'elixer'),(0,3,'flask'),(0,4,'scroll'),(0,5,'food'),(0,6,'item enhancement'),(0,7,'bandage'),(0,8,'consumable other'),(1,0,'container'),(1,1,'soul container'),(1,2,'herb container'),(1,3,'enchanting container'),(1,4,'engineering container'),(1,5,'gem container'),(1,6,'mining container'),(1,7,'leatherworking container'),(1,8,'inscription container'),(2,0,'axe'),(2,1,'axe2'),(2,2,'bow'),(2,3,'gun'),(2,4,'mace'),(2,5,'mace2'),(2,6,'polearm'),(2,7,'sword'),(2,8,'sword2'),(2,9,'obsolete'),(2,10,'staff'),(2,11,'exotic'),(2,12,'exotic2'),(2,13,'fist'),(2,14,'misc'),(2,15,'dagger'),(2,16,'thrown'),(2,17,'spear'),(2,18,'crossbow'),(2,19,'wand'),(2,20,'fishing pole'),(3,0,'red'),(3,1,'blue'),(3,2,'yellow'),(3,3,'purple'),(3,4,'green'),(3,5,'orange'),(3,6,'meta'),(3,7,'simple'),(3,8,'prismatic'),(4,0,'misc'),(4,1,'cloth'),(4,2,'leather'),(4,3,'mail'),(4,4,'plate'),(4,5,'buckler'),(4,6,'shield'),(4,7,'libram'),(4,8,'idol'),(4,9,'totem'),(4,10,'sigil'),(5,0,'reagent'),(6,0,'wand'),(6,1,'bolt'),(6,2,'arrow'),(6,3,'bullet'),(6,4,'thrown'),(7,0,'trade goods'),(7,1,'parts'),(7,2,'explosives'),(7,3,'devices'),(7,4,'jewelcrafting'),(7,5,'cloth'),(7,6,'leather'),(7,7,'metal stone'),(7,8,'meat'),(7,9,'herb'),(7,10,'elemental'),(7,11,'trade goods other'),(7,12,'enchanting'),(7,13,'material'),(7,14,'armor enhancement'),(7,15,'weapon enchancement'),(8,0,'generic'),(9,0,'book'),(9,1,'leatherworking pattern'),(9,2,'tailoring pattern'),(9,3,'engineering schematic'),(9,4,'blacksmithing'),(9,5,'cooking recipe'),(9,6,'alchemy recipe'),(9,7,'first aid manual'),(9,8,'enchanting formula'),(9,9,'fishing manual'),(9,10,'jewelcrafting recipe'),(10,0,'money'),(11,0,'quiver0'),(11,1,'quiver1'),(11,2,'quiver'),(11,3,'ammo pouch'),(12,0,'quest'),(13,0,'key'),(13,1,'lockpick'),(14,0,'permanent'),(15,0,'junk'),(15,1,'junk reagent'),(15,2,'junk pet'),(15,3,'junk holiday'),(15,4,'junk other'),(15,5,'junk_mount'),(16,1,'warrior'),(16,2,'paladin'),(16,3,'hunter'),(16,4,'rougue'),(16,5,'priest'),(16,6,'death knight'),(16,7,'shaman'),(16,8,'mage'),(16,9,'warlock'),(16,11,'druid');
/*!40000 ALTER TABLE `auctionator_item_class` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `auctionator_item_quality`
--

DROP TABLE IF EXISTS `auctionator_item_quality`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `auctionator_item_quality` (
  `quality` tinyint NOT NULL COMMENT 'Quality value of the item',
  `name` varchar(100) COLLATE utf8mb4_general_ci DEFAULT NULL,
  PRIMARY KEY (`quality`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci COMMENT='Quality mappings from native int value to friendly name';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `auctionator_item_quality`
--

LOCK TABLES `auctionator_item_quality` WRITE;
/*!40000 ALTER TABLE `auctionator_item_quality` DISABLE KEYS */;
INSERT INTO `auctionator_item_quality` VALUES (0,'poor'),(1,'normal'),(2,'uncommon'),(3,'rare'),(4,'epic'),(5,'legendary'),(6,'artifact'),(7,'heirloom');
/*!40000 ALTER TABLE `auctionator_item_quality` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-09-13 21:36:07
