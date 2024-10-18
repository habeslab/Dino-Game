import 'dart:async';
import 'dart:io' as io;
import 'package:dino_game_leadboard/model/player.dart';
import 'package:path/path.dart';
import 'package:sqflite/sqflite.dart';
import 'package:path_provider/path_provider.dart';

class DBHelper {
  static final DBHelper _instance = DBHelper.internal();
  factory DBHelper() => _instance;

  static Database? _db;

  DBHelper.internal();

  Future<Database?> get db async {
    if (_db != null) {
      return _db;
    }
    _db = await initDb();
    return _db;
  }

  Future<Database?> initDb() async {
    io.Directory documentsDirectory = await getApplicationDocumentsDirectory();
    String path = join(documentsDirectory.path, "main.db");
    var theDb = await openDatabase(path, version: 1, onCreate: (Database db, int version) async {
      await db.execute('''
        CREATE TABLE players (
          id INTEGER PRIMARY KEY AUTOINCREMENT,
          firstName TEXT NOT NULL,
          lastName TEXT NOT NULL,
          score INTEGER NOT NULL
        )
      ''');
    });
    return theDb;
  }

  Future<Player?> insert(Player player) async {
    var dbClient = await db;
    if (dbClient == null) return null;
    player.id = await dbClient.insert('players', player.toMap());
    return player;
  }

  Future<Player?> getPlayer(int id) async {
    var dbClient = await db;
    if (dbClient == null) return null;
    List<Map> maps = await dbClient.query('players',
        columns: ['id', 'firstName', 'lastName', 'score'],
        where: 'id = ?',
        whereArgs: [id]);
    if (maps.isNotEmpty) {
      return Player.fromMap(maps.first);
    }
    return null;
  }

  Future<int> delete(int id) async {
    var dbClient = await db;
    if (dbClient == null) return 0;
    return await dbClient.delete('players', where: 'id = ?', whereArgs: [id]);
  }

  Future<int> deleteAll() async {
    var dbClient = await db;
    if (dbClient == null) return 0;
    return await dbClient.delete('players'); // Questa cancellazione non specifica un where, quindi elimina tutti i record
}

  Future<int> update(Player player) async {
    var dbClient = await db;
    if (dbClient == null) return 0;
    return await dbClient.update('players', player.toMap(),
        where: 'id = ?', whereArgs: [player.id]);
  }

  Future<List<Player>> getAllPlayers() async {
    var dbClient = await db;
    if (dbClient == null) return [];
    List<Map> maps = await dbClient.query('players',
        columns: ['id', 'firstName', 'lastName', 'score'], orderBy: 'score DESC');
    return List.generate(maps.length, (i) {
      return Player.fromMap(maps[i]);
    });
  }

  Future close() async {
    var dbClient = await db;
    if (dbClient != null) {
      await dbClient.close();
    }
  }
}
