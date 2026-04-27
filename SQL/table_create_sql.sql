CREATE TABLE `device_node` (
  `id` BIGINT NOT NULL AUTO_INCREMENT COMMENT '节点ID',
  `node_name` VARCHAR(64) NOT NULL COMMENT '节点名称',
  `ip_address` VARCHAR(64) NOT NULL DEFAULT '' COMMENT '节点IP地址',
  `mac_address` VARCHAR(64) NOT NULL DEFAULT '' COMMENT '物理MAC地址',
  `status` TINYINT NOT NULL DEFAULT 0 COMMENT '节点状态: 0-离线, 1-在线, 2-故障, 3-维护中',
  `last_heartbeat_time` DATETIME(3) NULL COMMENT '最后一次心跳时间',
  
  `create_time` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3) COMMENT '注册时间',
  `update_time` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3) ON UPDATE CURRENT_TIMESTAMP(3) COMMENT '信息更新时间',
  `is_deleted` TINYINT(1) NOT NULL DEFAULT 0 COMMENT '软删除标记: 0-正常, 1-已删除',
  
  PRIMARY KEY (`id`),
  UNIQUE KEY `uk_mac_address` (`mac_address`),
  KEY `idx_status` (`status`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='边缘视觉节点管理表';

CREATE TABLE `system_log` (
  `id` BIGINT NOT NULL AUTO_INCREMENT COMMENT '日志主键',
  `node_id` BIGINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '关联节点ID (0表示系统全局日志)',
  
  `log_level` TINYINT NOT NULL DEFAULT 1 COMMENT '级别: 1-INFO, 2-WARN, 3-ERROR, 4-FATAL',
  `module` VARCHAR(64) NOT NULL DEFAULT '' COMMENT '模块名称',
  `content` VARCHAR(1024) NOT NULL COMMENT '日志具体内容',
  
  `created_time` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3) COMMENT '日志产生时间',
  
  PRIMARY KEY (`id`),
  KEY `idx_node_time_level` (`node_id`, `created_time`, `log_level`),
  KEY `idx_created_time` (`created_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='系统运行日志表';

CREATE TABLE `defect_record` (
  `id` BIGINT NOT NULL AUTO_INCREMENT COMMENT '次品记录ID',
  `node_id` BIGINT UNSIGNED NOT NULL COMMENT '节点ID',
  `batch_number` VARCHAR(64) NOT NULL DEFAULT '' COMMENT '生产批次号',
  
  `defect_type` VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN' COMMENT '缺陷类型',
  
  `image_path` VARCHAR(255) NOT NULL COMMENT '物理硬盘图片相对路径',
  `thumbnail_path` VARCHAR(255) NOT NULL DEFAULT '' COMMENT '缩略图路径',
  
  `review_status` TINYINT NOT NULL DEFAULT 0 COMMENT '人工复核状态: 0-待复核, 1-确认次品, 2-误报放行',
  `reviewer_id` BIGINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '复核人ID',
  
  `capture_time` DATETIME(3) NOT NULL COMMENT '相机的实际抓拍时间',
  `created_time` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3) COMMENT '入库时间',
  `updated_time` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3) ON UPDATE CURRENT_TIMESTAMP(3) COMMENT '复核更新时间',
  
  PRIMARY KEY (`id`),
  KEY `idx_batch_number` (`batch_number`),
  KEY `idx_node_status_time` (`node_id`, `review_status`, `capture_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='次品影像追踪表';

CREATE TABLE `total_productions` (
	`id` BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT COMMENT '主键',
	`count` BIGINT NOT NULL DEFAULT 0 COMMENT '产品总数'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='产品总数表';