//All Test# (#-letter), will be loaded from file when dev will over

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp> 
#include <iostream>
#include <thread>
#include <vector>
#include <string>

//#define DEBUG
//#define DEBUGHEALTH
// #define THREADING // НЕ НАДО

using namespace sf;

const Vector2f V2fNull{ 0.f, 0.f };
const Vector2i V2iNull{ 0, 0 };

float InputAxisX = 0;
float InputAxisY = 0;

bool onPlayerTransition = false;

Clock UpdateClock;
Clock AnimUpdateClock;

template <typename T>
class AdvList;
template <typename T1, typename T2, typename T3>
class Orfey;

class Denizen;
class Point;
class Item;
class Weapon;
class Camera;
class Effect;
class AttackingEffect;
class Map;

int playerId = 0;

template <typename T>
T Clamp(T A, T Min, T Max) {
	if (A > Max) return Max;
	else if (A < Min) return Min;
	else return A;
}

template <typename T>
class AdvList
{

private:
	class Element {
	public:
		int id = 0;
		T* value;
		Element* nextElement;
		Element(T& value, Element& nextElement)
		{
			this->value = value;
			this->nextElement = nextElement;
		};
		Element(T& value)
		{
			this->value = value;
			this->nextElement = nullptr;
		};
		Element()
		{
			this->value = nullptr;
			this->nextElement = nullptr;
		};
	};

	Element* firstElement = new Element;

public:
	AdvList() {};
	~AdvList() { Clear(); };
	void Add(T& element) {
		size++;
		if (firstElement == nullptr) firstElement = new Element;
		if (firstElement->value == nullptr) { firstElement->value = &element; }
		else {
			int newId = 1;
			Element* buf;
			buf = firstElement;
			while (buf->nextElement != nullptr) {
				buf = buf->nextElement;
				newId++;
			}
			if (buf->nextElement == nullptr) {
				Element* newElem = new Element;
				newElem->value = &element;
				buf->nextElement = newElem;
				newElem->id = newId;
#ifdef DEBUGLIST
				std::cout << "Added " << newElem->id << std::endl;
#endif // DEBUGLIST
			}
		}
	}
	Element& GetFirst() {
		return *firstElement;
	}
	Element& GetLast() {
		return GetById(size - 1);
	}
	Element& GetById(int id) {
		Element* buf = firstElement;

		for (int i = 0; i < id + 1; i++) {
			if (buf == nullptr) return *firstElement;

			if (buf->id == id) return *buf;
			else buf = buf->nextElement;
		}
		return *firstElement;
	}
	void RemoveById(int id) {
		Element* buf = firstElement;
		Element* Temp;
		if (id == 0) {
			firstElement = buf->nextElement;
			delete buf;
			size--;
			buf = firstElement;
			for (int k = 0; k < size; k++) {
				buf->id = k;
				buf = buf->nextElement;
			}
#ifdef DEBUGLIST
			std::cout << "Removed " << id << std::endl;
#endif// DEBUGLIST
			return;
		}
		for (int i = 0; i < id; i++) {
			if (buf->nextElement->id == id)
			{
				Temp = buf->nextElement;
				buf->nextElement = Temp->nextElement;
				delete Temp;
				size--;
				for (int k = ((buf->id) + 1); k < size; k++) {
					buf->nextElement->id--;
					buf = buf->nextElement;
				}
#ifdef DEBUGLIST
				std::cout << "Removed " << id << std::endl;
#endif // DEBUGLIST
				return;
			}
			buf = buf->nextElement;
		}
	};
	void Clear() {
		Element* buf;
		Element* Temp;

		Temp = firstElement;
		if (Temp == nullptr || Temp->value == nullptr) return;
		do {
#ifdef DEBUGLIST
			std::cout << "Cleared " << Temp->id << std::endl;
#endif	//DEBUGLIST
			buf = Temp;
			Temp = buf->nextElement;
			delete buf;
		} while (Temp->nextElement != nullptr);

	}
	int Size() {
		return size;
	}
private:
	int size = 0;


};

class Map
{
private:
	Texture spriteTexture;
	class Tile
	{
	public:
		int mapSizeX, mapSizeY;
		int sizeX = 0;
		int sizeY = 0;
		Sprite sprite;
		//Rect<float> initRect;

		Tile() {};

		void loadSpriteTexture(int sizeX, int sizeY, int coordX, int coordY, float scale, Texture& spriteTexture) {
			sprite.setTexture(spriteTexture); sprite.setTextureRect(IntRect(coordX, coordY, sizeX, sizeY)); sprite.setScale(scale, scale);
			//initRect.width = sprite.getLocalBounds().width + 1.0f;
			//initRect.height = sprite.getLocalBounds().height + 1.0f;
			//initRect.left = sprite.getGlobalBounds().left - 1.0f;
			//initRect.top = sprite.getGlobalBounds().top - 1.0f;
		};
		void Set(int sizeX, int sizeY,
			Texture& spriteTexture,
			int posXOnTileset, int posYOnTileset,
			float newPositionX, float newPositionY,
			float scale)
		{
			this->sizeX = sizeX; this->sizeY = sizeY;
			loadSpriteTexture(sizeX, sizeY, posXOnTileset, posYOnTileset, scale, spriteTexture);
			Move(newPositionX, newPositionY);
		};
		~Tile() {};
		void Move(float posX, float posY) {
			sprite.setPosition(posX, posY);
			//initRect.left = sprite.getGlobalBounds().left - 1.0f;
			//initRect.top = sprite.getGlobalBounds().top - 1.0f;
		};
		bool CheckOnCollapseX(Sprite& object, float& Speed) {
			if ((this->sprite.getGlobalBounds().intersects(object.getGlobalBounds()))
				&& ((this->sprite.getGlobalBounds().left + Speed >= (object.getGlobalBounds().left + object.getGlobalBounds().width))
					|| ((this->sprite.getGlobalBounds().left +
						this->sprite.getGlobalBounds().width - Speed) <= object.getGlobalBounds().left))
				&& !((this->sprite.getGlobalBounds().top + Speed >= (object.getGlobalBounds().top + object.getGlobalBounds().height))
					|| ((this->sprite.getGlobalBounds().top +
						this->sprite.getGlobalBounds().height - Speed) <= object.getGlobalBounds().top))) {
				return true;
			}
			else return false;
		};
		bool CheckOnCollapseY(Sprite& object, float& Speed) {
			if ((this->sprite.getGlobalBounds().intersects(object.getGlobalBounds()))
				&& !((this->sprite.getGlobalBounds().left + Speed >= (object.getGlobalBounds().left + object.getGlobalBounds().width))
					|| ((this->sprite.getGlobalBounds().left +
						this->sprite.getGlobalBounds().width - Speed) <= object.getGlobalBounds().left))
				&& ((this->sprite.getGlobalBounds().top + Speed >= (object.getGlobalBounds().top + object.getGlobalBounds().height))
					|| ((this->sprite.getGlobalBounds().top +
						this->sprite.getGlobalBounds().height - Speed) <= object.getGlobalBounds().top))) {
				return true;
			}
			else return false;
		}
	};
	std::vector<std::vector<Tile>> Tiles;
	std::vector<Tile*> BlockTiles;
public:
	int tileSizeX, tileSizeY;
	int blockTilesCount = 0;
	Map(int Pattern[32][32], int sizeX, int sizeY, std::string path = "") {		int counter = 0;
		loadTilesetTexture(path);
		tileSizeX = sizeX;
		tileSizeY = sizeY;
		Tiles.resize(32);
		BlockTiles.resize(32 * 32);
		for (int i = 0; i < Tiles.size(); i++) {
			Tiles.at(i).resize(32);
		}

		for (int i = 0; i < Tiles.size(); i++) {
			for (int k = 0; k < Tiles.at(i).size(); k++) {
				Tiles.at(i).at(k).Set(sizeX, sizeY, spriteTexture, Pattern[i][k] * sizeX, 0, k * sizeX - 96.0f, i * sizeY - 96.0f, 1.0f);
				if (Pattern[i][k] != 0) {
					BlockTiles.at(counter) = &Tiles.at(i).at(k);
					counter++;
#ifdef DEBUGWALLS
					std::cout << "Wall " << counter << "\t value: " << Pattern[i][k] << " i: " << i << " k: " << k << std::endl;
#endif // DEBUGWALLS
				}
			}
		}
		blockTilesCount = counter;
	};
	void loadTilesetTexture(std::string path) {
		try {
			if (!spriteTexture.loadFromFile(path)) throw(1);
			else { spriteTexture.loadFromFile(path); }
		}
		catch (const int ex) {
			std::cout << (char)7 << "Failed to load weapon icon " << ex << std::endl;
		}
	};
	bool CheckCollapseX(Sprite* sprite, float* Speed) {
		for (int i = 0; i < blockTilesCount; i++) {
			if (getBlockTileById(i)->CheckOnCollapseX(*sprite, *Speed))
				return true;
		}
		return false;
	}
	bool CheckCollapseY(Sprite* sprite, float* Speed) {
		for (int i = 0; i < blockTilesCount; i++) {
			if (getBlockTileById(i)->CheckOnCollapseY(*sprite, *Speed))
			{
				return true;
			}
		}
		return false;
	}
	Sprite& getTileById(int indexX, int indexY) {
		return Tiles.at(indexY).at(indexX).sprite;
	}
	Tile* getBlockTileById(int index) {
		return BlockTiles.at(index);
	}
	~Map() {
		for (int i = 0; i < Tiles.size(); i++) {
			for (int k = 0; k < Tiles.at(i).size(); k++) {
				delete& Tiles.at(i).at(k);
			}
		}
	};

};

template <typename T1, typename T2, typename T3>
class Orfey
{
public:
	Orfey() {};
	~Orfey() {
	};
	T1& AddDenizen(T1& Denizen) { Denizens.Add(Denizen); return *(Denizens.GetLast().value); };
	T2& AddItem(T2& Item) { Items.Add(Item); return *(Items.GetLast().value); };
	T3& AddEffect(T3& Effect) { Effects.Add(Effect); return *(Effects.GetLast().value); };
	void RemoveDenizenById(int index) {
		if (playerId == index) { playerId = Denizens.Size() - 2; }
		else if (index != 0 && playerId != 0) { playerId--; }
		Denizens.RemoveById(index);
	};
	void RemoveItemById(int index) { Items.RemoveById(index); };
	void RemoveEffectById(int index) { Effects.RemoveById(index); };
	T1& GetDenizenById(int index) { return *(Denizens.GetById(index).value); };
	T2& GetItemById(int index) { return *(Items.GetById(index).value); };
	int GetSoulsCount() { return Denizens.Size(); };
	int GetItemsCount() { return Items.Size(); };
	T3& GetEffectById(int index) { return *(Effects.GetById(index).value); };
	int GetEffectsCount() { return Effects.Size(); };
	Map* GetMap() { return MyMap; };
	void SetMap(Map* Map) { this->MyMap = Map; };
private:
	enum ListType { Denizen, Item };
	AdvList<T1> Denizens;
	AdvList<T2> Items;
	AdvList<T3> Effects;
	Map* MyMap = nullptr;
};

Orfey<Denizen, Item, Effect> BEHOLDTHEWORLD;
class Point {
public:
	Point() {};
	Point(Vector2f position) {
		SetPosition(position);
	}
	void SetPosition(Vector2f NewPosition) {
		position = NewPosition;
	}
	void Move(Vector2f Offset) {
		position += Offset;
	}
	Vector2f position;
};
struct AnimContainer {

	int spriteSizeX;
	int spriteSizeY;
	float animFrameTime;
	int maxPos;
	AnimContainer(int currentAnimKey = 0, int spriteSizeX = 0, int spriteSizeY = 0, float animFrameTime = 0.0f, int maxPos = 0) {
		Set(currentAnimKey, spriteSizeX, spriteSizeY, animFrameTime, maxPos );
	};

	void Set(int currentAnimKey, int spriteSizeX, int spriteSizeY, float animFrameTime, int maxPos ) {
		this->spriteSizeX = spriteSizeX;
		this->spriteSizeY = spriteSizeY;
		this->animFrameTime = animFrameTime;
		this->maxPos = maxPos; 
	};
};
struct AttackCharContainer {

	enum RangaRegenType { perPunch, perSec };

	float damage = 0.0f;

	float healthRegenSp = 3.0f;
	float rangaConsumeSp = 0.0f;
	float rangaRegenSp = 0.0f;

	float attackCooldown = 0.0f;
	RangaRegenType rangaRegenType = perPunch;

	AttackCharContainer(float damage = 0.0f, float healthRegenSp = 0.0f, float rangaRegenSp = 0.0f, float rangaConsumeSp = 0.0f, float attackCooldown = 0.0f, RangaRegenType rangaRegenType = perPunch)
	{
		Set(damage, healthRegenSp, rangaRegenSp, rangaConsumeSp, attackCooldown, rangaRegenType);
	};
	void Set(float damage = 0.0f, float healthRegenSp = 0.0f, float rangaRegenSp = 0.0f, float rangaConsumeSp = 0.0f, float attackCooldown = 0.0f, RangaRegenType rangaRegenType = perPunch) {
		this->damage = damage;
		this->healthRegenSp = healthRegenSp;
		this->rangaRegenSp = rangaRegenSp;
		this->rangaConsumeSp = rangaConsumeSp;
		this->rangaRegenType = rangaRegenType;
		this->attackCooldown = attackCooldown;
	};
};

struct ITexturable {
	Sprite& GetSprite() { return this->sprite; };
protected:
	Texture spriteTexture;
	Sprite sprite;

	virtual void loadTexture(std::string path) {
		try {
			if (!spriteTexture.loadFromFile(path)) throw(1);
			else { spriteTexture.loadFromFile(path);  sprite.setTexture(spriteTexture); }
		}
		catch (const int ex) {
			std::cout << (char)7 << "Failed to load weapon texture " << ex << std::endl;
		}
	};
	virtual void loadTexture(Sprite& sprite, Texture& texture, std::string path) {
		try {
			if (!texture.loadFromFile(path)) throw(1);
			else { texture.loadFromFile(path);  sprite.setTexture(texture); }
		}
		catch (const int ex) {
			std::cout << (char)7 << "Failed to load weapon texture " << ex << std::endl;
		}
	};
	bool multiAnim = false;
};
struct IAnimable : public virtual ITexturable {
	enum SideState { Left = 1, Right = 2, Up = 3, Down = 4};

	SideState& GetCurrentSideState() { return currentSideState; }

	virtual int UpdateAnim(bool onNeed = false) {
		if (!onNeed && (!(animClock.getElapsedTime().asMilliseconds() >= animations.at(currentAnimKey).animFrameTime))) return 2;
		if (currentSideState > 2)
			this->sprite.setTextureRect(IntRect(
				animations.at(currentAnimKey).spriteSizeY * currentPos,
				animations.at(currentAnimKey).spriteSizeX * (currentAnimKey * 4 * multiAnim + currentSideState - 3) + animations.at(currentAnimKey).spriteSizeY*2,
				animations.at(currentAnimKey).spriteSizeY,
				animations.at(currentAnimKey).spriteSizeX));
		if (currentSideState <3 ) 
		this->sprite.setTextureRect(IntRect(
			animations.at(currentAnimKey).spriteSizeX * currentPos,
			animations.at(currentAnimKey).spriteSizeY * (currentAnimKey * 4 * multiAnim + currentSideState - 1),
			animations.at(currentAnimKey).spriteSizeX,
			animations.at(currentAnimKey).spriteSizeY));

		animClock.restart();
		if (onNeed) return 2;
		if (currentPos < animations.at(currentAnimKey).maxPos) { currentPos++; return 1; }

		EndAnim();
		return 0;
	};
	IAnimable(std::vector<AnimContainer>* animations = nullptr,bool multiAnim = false) {
		this->animations = *animations;
		this->multiAnim = multiAnim;
	};

protected:
	int currentPos = 0;
	void PosToZero() { if (currentPos != 0) currentPos = 0; }

	std::vector<AnimContainer> animations;
	Clock animClock;

	int currentAnimKey = 0;

	SideState currentSideState = Down;
	void SetSideState(SideState sideState) {
		if (currentSideState == sideState) return;
		currentSideState = sideState;
		PosToZero();
	};
	void EndAnim()
	{
		PosToZero();
		animClock.restart();
	}
	void loadTexture(std::string path) override {
		ITexturable::loadTexture(path);
		UpdateAnim(true);
	};
	void loadTexture(Sprite& sprite, Texture& texture, std::string path) override {
		ITexturable::loadTexture(sprite, texture, path);
		UpdateAnim(true);
	};
};
struct EffectContainer
{
	std::vector<AnimContainer> animations;
	std::string path;
	std::string pathAvance = "";
	EffectContainer() {
	};
	void Set(std::string path = "", std::vector<AnimContainer>* animations = nullptr) {
		this->path = path;
		this->animations = *animations;
	};
	void Set(std::string path = "",
		std::string pathAvance = "",
		std::vector<AnimContainer>* animations = nullptr) {
		this->path = path;
		this->pathAvance = pathAvance;
		this->animations = *animations;
	};
};

class Denizen : public IAnimable
{
public:
protected:
	int id;

	float maxHealth = 100.0f, currentHealth, healthRegenMultiplier = 1.0f, HealthRegenBorder = maxHealth;
	bool isAlive = true;

	float maxRanga, currentRanga;

	float maxSp;

	Item* weapon;
	bool isAttacking = false; 

public:
	Denizen(float maxHealth = 100.0f, float healthRegenMultiplier = 1.0f,
		float maxRanga = 100.0f,
		float maxSp = 1.0f,
		std::string path = "",
		float coordX = 0, float coordY = 0,
		Orfey<Denizen, Item, Effect>* THEWORLD = nullptr,
		std::vector<AnimContainer>* animations = nullptr, bool multiAnim = true) : IAnimable(animations,multiAnim)
	{
		
		this->maxHealth = maxHealth;
		this->currentHealth = this->maxHealth;
		this->healthRegenMultiplier = healthRegenMultiplier;
		this->HealthRegenBorder = this->maxHealth;
		
		this->maxRanga = maxRanga;
		currentRanga = 0.0f;

		this->maxSp = maxSp;

		loadTexture(path);
		this->SetPosition(coordX, coordY);
		this->id = THEWORLD->GetSoulsCount();

	};
	Denizen(float maxHealth = 100.0f, float healthRegenMultiplier = 1.0f,
		float maxRanga = 100.0f,
		float maxSp = 1.0f,
		std::string path = "",
		float coordX = 0, float coordY = 0,
		Orfey<Denizen, Item, Effect>* THEWORLD = nullptr,
		Item* weapon = nullptr,
		std::vector<AnimContainer>* animations = nullptr, bool multiAnim = true) : IAnimable(animations, multiAnim)
	{
		//В ответе за хп
		this->maxHealth = maxHealth;
		this->currentHealth = this->maxHealth;
		this->healthRegenMultiplier = healthRegenMultiplier;
		this->HealthRegenBorder = this->maxHealth;
		//В ответе за рэнжу
		this->maxRanga = maxRanga;
		this->maxSp = maxSp;
		currentRanga = 0.0f;

		loadTexture(path);
		this->SetPosition(coordX, coordY);
		//В ответе за место в мире
		this->id = THEWORLD->GetSoulsCount();
		//В ответе за оружие
		this->weapon = weapon;
		ClaimWeapon(this->weapon);
	};

	~Denizen();

	void GetDamage(float damage) {
		currentHealth = Clamp(currentHealth - damage, 0.0f, maxHealth);
		HealthRegenBorder = Clamp(currentHealth * healthRegenMultiplier, currentHealth, maxHealth);
#ifdef DEBUGHEALTH
		std::cout << id << "  " << currentHealth << std::endl;//кол-во хп
#endif // DEBUGHEALTH

		if (currentHealth <= 0.0f) {
#ifdef DEBUGHEALTH
			std::cout << id << " Dead" << std::endl;//Жив ли
#endif //DEBUGHEALTH
			isAlive = false;
			delete this;
		}
	};
	void ClaimWeapon(Item* weapon);//по какой-то причине показывает неопределённость функции, когда она определена

	void Heal(float RegenCount) {
		currentHealth = Clamp(currentHealth + RegenCount, currentHealth, HealthRegenBorder);
#ifdef DEBUGHEALTH
		std::cout << id << " Healed " << currentHealth << std::endl;
#endif // DEBUGHEALTH
	};
	void RangaRegen(float RegenCount) {
		currentRanga = Clamp(currentRanga + RegenCount, currentRanga, maxRanga);
#ifdef DEBUGRANGA
		std::cout << id << " RangaRegened " << currentRanga << std::endl;
#endif // DEBUGRANGA
	};
	void RangaConsume(float ConsumeCount) {
		currentRanga = Clamp(currentRanga - ConsumeCount, 0.0f, maxRanga);
#ifdef DEBUGRANGA
		std::cout << id << " RangaConsumed " << currentRanga << std::endl;
#endif // DEBUGRANGA
	};
	void StartAttack(int currentAnimKey);
	void StartPreview(int currentAnimKey, bool start);
	void StopAttack() { isAttacking = false; };
	float GetHealthCount() { return currentHealth; };
	float GetRangaCount() { return currentRanga; };
	void Move(float inputX, float inputY) {
		sprite.move(maxSp * inputX, -1.0f * maxSp * inputY);
		MoveWeapon(maxSp * inputX, -1.0f * maxSp * inputY);
		if (BEHOLDTHEWORLD.GetMap()->CheckCollapseY(&sprite, &maxSp)) { sprite.move(0.0f, maxSp * inputY); MoveWeapon(0.0f, maxSp * inputY); }
		if (BEHOLDTHEWORLD.GetMap()->CheckCollapseX(&sprite, &maxSp)) { sprite.move(maxSp * inputX * -1.0f, 0.0f); MoveWeapon(maxSp * inputX * -1.0f, 0.0f); }
	};
	void MoveWeapon(float offsetX, float offsetY);
	void SetPosition(float x, float y) {
		sprite.setPosition(x, -1.0f * y);
	}

	void MoveStateChange(float InputAxisX, float InputAxisY)//когда будет анимация атаки и т.д. -> переделать 
	{
		currentAnimKey = 1;
		if (InputAxisX > 0.0f) {
			return SetSideState(Right);
		}
		else if (InputAxisX < 0.0f) {
			return SetSideState(Left);
		}
		if (InputAxisY != 0) {
			if (InputAxisY < 0.0f) {
				return SetSideState(Down);
			}
			return SetSideState(Up);
		}
		currentAnimKey = 0;
	};
	int UpdateAnim(bool onNeed = false) override {
		IAnimable::UpdateAnim(onNeed);
		return 0;
	};


	int GetId() { return id; };

};
//Вещь
class Item : public virtual ITexturable
{
protected:
	Texture iconTexture;

	Sprite icon;

	Denizen* carrier = nullptr;

	bool onUse = false;

	std::string Name = "PachKa CigapeT";
public:
	Item() {};
	~Item() {};
	void GainCarrier(Denizen* newCarrier) { carrier = newCarrier; };
	void LostCarrier() { carrier = nullptr; }

	bool& OnUse() { return onUse; };

	virtual int UpdateAnim(bool onNeed = false) { return 0; };
	virtual void Use(int currentAnimKey) {};
	virtual void PlayPreview(int currentAnimKey, bool start) {};
	virtual void MoveEffect(Vector2f offset) {};
	virtual int SetSideState(IAnimable::SideState sideState) { return 0; };
};
//Эффект
class Effect : public IAnimable
{
public:
	Effect(float posX = 0, float posY = 0, EffectContainer* effectContainer = nullptr,bool multiAnim = false)
		: IAnimable(&(effectContainer->animations),multiAnim) {
		loadTexture(effectContainer->path);
		currentAnimKey = 0;
		this->sprite.setPosition(posX, posY);
		this->id = BEHOLDTHEWORLD.GetEffectsCount();
	};
	Effect(float posX = 0, float posY = 0, std::string path = "", std::vector<AnimContainer>* animations = nullptr, bool multiAnim = false)
		: IAnimable(animations,multiAnim) {
		loadTexture(path);
		currentAnimKey = 0;
		this->sprite.setPosition(posX, posY);
		this->id = BEHOLDTHEWORLD.GetEffectsCount();
	};
	~Effect() { BEHOLDTHEWORLD.RemoveEffectById(id); };
	void virtual SetMode(int mode) {};
	void virtual SetPosition(Vector2f position) { this->sprite.setPosition(position); };
	void virtual Move(Vector2f position) { this->sprite.setPosition(this->sprite.getPosition() + position); };
	int virtual GetMode() { return 1; };
	int SetSideState(IAnimable::SideState sideState) {
		if (currentSideState == sideState) return 1;
		this->currentSideState = sideState;
		return 0;
	}
protected:
	int id = 0;

};
//Атакующий эффект
class AttackingEffect : public Effect {
public:
	AttackingEffect(float posX, float posY,
		EffectContainer* effectContainer = nullptr,
		int animKeyCode = 0,
		Weapon* summoner = nullptr,
		IAnimable::SideState currentSideState = IAnimable::Right)
		: Effect(posX, posY, effectContainer) {
		this->path = effectContainer->path;
		this->pathAvance = effectContainer->pathAvance;
		this->currentSideState = currentSideState;
		this->currentAnimKey = animKeyCode;
		this->summoner = summoner;
	};
	AttackingEffect(float posX, float posY,
		std::string path = "",
		std::string pathAvance = "",
		std::vector<AnimContainer>* animations = nullptr,
		int animKeyCode = 0,
		Weapon* summoner = nullptr,
		IAnimable::SideState currentSideState = IAnimable::Right)
		: Effect(posX, posY, path, animations) {
		this->path = path;
		this->pathAvance = pathAvance;
		this->currentSideState = currentSideState;
		this->currentAnimKey = animKeyCode;
		this->summoner = summoner;
	};
	~AttackingEffect();
	int UpdateAnim(bool onNeed) override {
		if (onPreview) { IAnimable::UpdateAnim(onNeed); return 3; }

		switch (IAnimable::UpdateAnim(onNeed)) {
		case 0:
			delete this;
			return 1;
			break;
		case 1:

			CheckOnCollapse();
			return 0;
			break;
		}
		return 4;
	};
	void SetMode(int mode) override {
		switch (mode) {
		case 0:
			onPreview = false;
			loadTexture(path);
			break;
		case 1:
			onPreview = true;
			loadTexture(pathAvance);
			break;
		}
	}
	int GetMode() override {
		return (int)onPreview;
	}
protected:

	std::string path = "";
	std::string pathAvance = "";
	bool onPreview = false;
	int animKeycode = 0;
	Weapon* summoner = nullptr;
	void CheckOnCollapse();
};
//Оружие
class Weapon : public virtual Item, public virtual IAnimable
{
public:
protected:
	std::vector<AttackCharContainer> attackChars;
	std::vector<EffectContainer> effectContainers;
	//сократить количество контейнеров объединив в один
	struct AttackContainer
	{
		Effect* EffectInstance;
		Clock cooldownClock;
		Vector2f lastpos;
		bool canAttack;
		AttackContainer() {
			EffectInstance = nullptr;
			lastpos = V2fNull;
			canAttack = false;
		}
	};
	std::vector<AttackContainer> attacks;
	int previewId = 0;
public:
	Denizen& GetCarrier() { return *carrier; };
	AttackCharContainer& GetAttackChar(int keyCode) { return attackChars.at(keyCode); };
	void SetEffectToNull(int id) { attacks.at(id).EffectInstance = nullptr; };
	Weapon(
		std::string spriteTexturePath = "icon.jpg",
		std::string iconPath = "icon.jpg",
		std::vector<AnimContainer>* animations = nullptr,
		std::vector<AttackCharContainer>* attackChars = nullptr,
		std::vector<EffectContainer>* effectContainers = nullptr
	) : IAnimable(animations)
	{
		for (int i = 0; i < attackChars->size(); i++) {
			attacks.push_back(*(new AttackContainer));
		}
		this->effectContainers = *effectContainers;
		this->attackChars = *attackChars;
		loadTexture(sprite, spriteTexture, spriteTexturePath);
		loadTexture(icon, iconTexture, iconPath);

	};
	~Weapon() {
		for (int i = 0;i < attacks.size();i++) { delete& attacks.at(i); attacks.pop_back(); }
	};

	void Use(int Keycode) override { Attack(Keycode); };
	void PlayPreview(int currentAnimKey, bool start) override {
		previewId = currentAnimKey;
		if ((attacks.at(currentAnimKey).EffectInstance != nullptr && start) || attacks.at(currentAnimKey).cooldownClock.getElapsedTime().asSeconds() < attackChars.at(currentAnimKey).attackCooldown) return;
		if (!start && attacks.at(currentAnimKey).EffectInstance != nullptr) return ClearEffect(currentAnimKey);
		if (!start) return;
		attacks.at(currentAnimKey).canAttack = true;
		switch (carrier->IAnimable::GetCurrentSideState()) {
		case IAnimable::Left:
			attacks.at(currentAnimKey).EffectInstance = (new AttackingEffect(
				carrier->GetSprite().getGlobalBounds().left - effectContainers.at(currentAnimKey).animations.at(currentAnimKey).spriteSizeX,
				carrier->GetSprite().getGlobalBounds().top,
				&effectContainers.at(currentAnimKey),
				currentAnimKey, this,
				carrier->IAnimable::GetCurrentSideState()));
			&BEHOLDTHEWORLD.AddEffect(*attacks.at(currentAnimKey).EffectInstance);
			attacks.at(currentAnimKey).EffectInstance->SetMode(1);
			SetSideState(IAnimable::Left);
			break;
		case IAnimable::Right:
			attacks.at(currentAnimKey).EffectInstance = (new AttackingEffect(
				carrier->GetSprite().getGlobalBounds().left + effectContainers.at(currentAnimKey).animations.at(currentAnimKey).spriteSizeX,
				carrier->GetSprite().getGlobalBounds().top,
				&effectContainers.at(currentAnimKey),
				currentAnimKey, this,
				carrier->IAnimable::GetCurrentSideState()));
			&BEHOLDTHEWORLD.AddEffect(*attacks.at(currentAnimKey).EffectInstance);
			attacks.at(currentAnimKey).EffectInstance->SetMode(1);
			SetSideState(IAnimable::Right);
			break;
		case IAnimable::Up:
			attacks.at(currentAnimKey).EffectInstance = (new AttackingEffect(
				carrier->GetSprite().getGlobalBounds().left,
				carrier->GetSprite().getGlobalBounds().top - effectContainers.at(currentAnimKey).animations.at(currentAnimKey).spriteSizeY,
				&effectContainers.at(currentAnimKey),
				currentAnimKey, this,
				carrier->IAnimable::GetCurrentSideState()));
			&BEHOLDTHEWORLD.AddEffect(*attacks.at(currentAnimKey).EffectInstance);
			attacks.at(currentAnimKey).EffectInstance->SetMode(1);
			SetSideState(IAnimable::Up);
			break;
		case IAnimable::Down:
			attacks.at(currentAnimKey).EffectInstance = (new AttackingEffect(
				carrier->GetSprite().getGlobalBounds().left,
				carrier->GetSprite().getGlobalBounds().top + effectContainers.at(currentAnimKey).animations.at(currentAnimKey).spriteSizeY,
				&effectContainers.at(currentAnimKey),
				currentAnimKey, this,
				carrier->IAnimable::GetCurrentSideState()));
			&BEHOLDTHEWORLD.AddEffect(*attacks.at(currentAnimKey).EffectInstance);
			attacks.at(currentAnimKey).EffectInstance->SetMode(1);
			SetSideState(IAnimable::Down);
			break;
		}

	};
	void ClearEffect(int id) {
		SetLastPos(id, attacks.at(id).EffectInstance->GetSprite().getGlobalBounds().getPosition());
		delete attacks.at(id).EffectInstance;
		SetEffectToNull(id);
	};
	void SetLastPos(int id, Vector2f pos) {
		attacks.at(id).lastpos = pos;
	};
	int SetSideState(IAnimable::SideState sideState) override {
		if (currentSideState == sideState) return 1;
		this->currentSideState = sideState;
		return 0;
	};
	void MoveEffect(Vector2f offset) override {
		if (attacks.at(previewId).EffectInstance == nullptr || attacks.at(previewId).EffectInstance->GetMode() == 0) return;
		attacks.at(previewId).EffectInstance->Move(offset);
		if (attacks.at(previewId).EffectInstance->SetSideState(currentSideState) != 1)
			switch (currentSideState) {
			case IAnimable::Left:
				attacks.at(previewId).EffectInstance->SetPosition(Vector2f(
					carrier->GetSprite().getGlobalBounds().left - effectContainers.at(previewId).animations.at(previewId).spriteSizeX,
					carrier->GetSprite().getGlobalBounds().top));
				break;
			case IAnimable::Right:
				attacks.at(previewId).EffectInstance->SetPosition(Vector2f(
					carrier->GetSprite().getGlobalBounds().left + effectContainers.at(previewId).animations.at(previewId).spriteSizeX,
					carrier->GetSprite().getGlobalBounds().top));
				break;
			case IAnimable::Up:
				attacks.at(previewId).EffectInstance->SetPosition(Vector2f(
					carrier->GetSprite().getGlobalBounds().left,
					carrier->GetSprite().getGlobalBounds().top - effectContainers.at(previewId).animations.at(previewId).spriteSizeY));
				break;
			case IAnimable::Down:
				attacks.at(previewId).EffectInstance->SetPosition(Vector2f(
					carrier->GetSprite().getGlobalBounds().left,
					carrier->GetSprite().getGlobalBounds().top + effectContainers.at(previewId).animations.at(previewId).spriteSizeY));
				break;
			};
	};
	/*int UpdateAnim(bool onNeed = false) override {
		if (!onUse || carrier == nullptr || carrier == NULL) return 1;
		return 3;
	};*/
	void Attack(int currentAnimKey) {
		if (onUse || carrier->GetRangaCount() < attackChars.at(currentAnimKey).rangaConsumeSp || attacks.at(currentAnimKey).EffectInstance != nullptr || !attacks.at(currentAnimKey).canAttack) return StopAttack();

		attacks.at(currentAnimKey).canAttack = false;
		carrier->RangaConsume(attackChars.at(currentAnimKey).rangaConsumeSp);

		onUse = true;
#ifdef DEBUGANIM
		std::cout << currentAnimKey << "weapon" << this->carrier->GetId() << "animkey" << std::endl;
#endif // DEBUGANIM

		//SetKeycode(currentAnimKey);

		attacks.at(currentAnimKey).EffectInstance = (new AttackingEffect(
			attacks.at(currentAnimKey).lastpos.x,
			attacks.at(currentAnimKey).lastpos.y,
			&effectContainers.at(currentAnimKey),
			currentAnimKey, this,
			carrier->IAnimable::GetCurrentSideState()));
		&BEHOLDTHEWORLD.AddEffect(*attacks.at(currentAnimKey).EffectInstance);
		attacks.at(currentAnimKey).EffectInstance->SetMode(0);
		SetSideState(carrier->IAnimable::GetCurrentSideState());

		StopAttack();

		attacks.at(currentAnimKey).cooldownClock.restart();
	};
private:
	void StopAttack()//заканчивает атаку
	{
		onUse = false;
		if (carrier == nullptr || carrier == NULL) return;
		carrier->StopAttack();
	}
	void SetKeycode(int currentAnimKey) {
		if (currentAnimKey) this->currentAnimKey = currentAnimKey;
	}
};
///////////////////////////////////////////////////
void Denizen::StartAttack(int currentAnimKey) {
	if (isAlive && !isAttacking) {
		isAttacking = true;
		weapon->Use(currentAnimKey);
	}
};

void Denizen::ClaimWeapon(Item* weapon) {
	weapon->GainCarrier(this);
};
void Denizen::StartPreview(int currentAnimKey, bool start) {
	if (isAlive && !isAttacking) {
		weapon->PlayPreview(currentAnimKey, start);
	}
};

void AttackingEffect::CheckOnCollapse() {
	for (int i = 0; i < BEHOLDTHEWORLD.GetSoulsCount(); i++) {
		if (sprite.getGlobalBounds().intersects(BEHOLDTHEWORLD.GetDenizenById(i).GetSprite().getGlobalBounds()) && &(BEHOLDTHEWORLD.GetDenizenById(i)) != &summoner->GetCarrier()) {
			BEHOLDTHEWORLD.GetDenizenById(i).GetDamage(summoner->GetAttackChar(animKeycode).damage);
			summoner->GetCarrier().Heal(summoner->GetAttackChar(animKeycode).healthRegenSp);
			summoner->GetCarrier().RangaRegen(summoner->GetAttackChar(animKeycode).rangaRegenSp);
		}
	}
};
AttackingEffect::~AttackingEffect() { summoner->SetEffectToNull(currentAnimKey); };

void Denizen::MoveWeapon(float offsetX, float offsetY) {
	if (weapon == nullptr) return;
	weapon->MoveEffect(Vector2f(offsetX, offsetY));
	weapon->SetSideState(currentSideState);
}
Denizen::~Denizen() { this->weapon->LostCarrier(); BEHOLDTHEWORLD.RemoveDenizenById(id); };
//Рисует
void Draw(RenderWindow& window);
//Осуществляет физику
void Move(RenderWindow& window);
//Меняет кадры
void Anim(RenderWindow& window);
//Камера
class Camera
{
public:
	Camera() {};
	Camera(Vector2f Center, Vector2f Size) {
		Set(Center, Size);
	};
	void Set(Vector2f Center, Vector2f Size) {
		ViewInstance.setCenter(Center);
		ViewInstance.setSize(Size);
	};
	~Camera() {};
	View& GetView() {
		return ViewInstance;
	}
	void Move(float deltaX, float deltaY) {
		ViewInstance.move(deltaX, deltaY);
	}
	void SetPosition(float posX, float posY) {
		ViewInstance.setCenter(posX, posY);
	}
	void Rotate(float angle) {
		ViewInstance.rotate(angle);
	}
private:
	View ViewInstance;
};

//Чисто тестовая -> удалить
int PatternTest[32][32]{
			{7 ,3 ,3 ,3 ,3 ,3 ,3 ,8 ,7 ,3 ,3 ,3 ,3 ,3 ,8 ,7 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,8 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,1 ,4 ,0 ,0 ,0 ,0 ,0 ,9 ,10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 ,10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 ,10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,5 ,6 ,0 ,0 ,0 ,0 ,0 ,9 ,10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,9 ,10,0 ,0 ,0 ,0 ,0 ,9 ,10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{13,11,11,11,11,11,11,12,13,11,6 ,0 ,0 ,5 ,12,10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{7 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,4 ,0 ,0 ,1 ,3 ,4 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{10,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,9 },
			{13,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,12} };

Camera MainCamera;


int main()
{
	setlocale(LC_ALL, "Rus");

	RenderWindow window(VideoMode(720, 450), L"TCG", Style::Default);

	window.setPosition(V2iNull);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);
	window.setActive(false);

	std::vector<AnimContainer>* TestW = new std::vector<AnimContainer>(2);
	std::vector<AttackCharContainer>* TestWA = new std::vector<AttackCharContainer>(2);
	std::vector<AnimContainer>* TestD = new std::vector<AnimContainer>(2); //для каждого префаба сделать свой
	TestW->at(0).Set(0, 48, 48, 50.0f, 7);
	TestW->at(1).Set(1, 48, 48, 50.0f, 13);
	TestWA->at(0) = *(new AttackCharContainer(2.0f, 3.0f, 5.0f, 0.0f, 1.0f, AttackCharContainer::perPunch));
	TestWA->at(1) = *(new AttackCharContainer(3.0f, 1.0f, 0.1f, 15.0f, 5.0f, AttackCharContainer::perPunch));
	TestD->at(0).Set(0, 48, 48, 200.0f, 2);
	TestD->at(1).Set(0, 48, 48, 200.0f, 2);
	std::vector<EffectContainer>* TestE = new std::vector<EffectContainer>(2);
	TestE->at(0).Set("meleetestattack.png", "meleetestattackweakpreview.png", TestW);
	TestE->at(1).Set("blowweak.png", "blowweakpreview.png", TestW);

	BEHOLDTHEWORLD.AddItem(*(new Weapon("attacks.png", "icon.jpg", TestW, TestWA, TestE)));
	BEHOLDTHEWORLD.AddItem(*(new Weapon("attacks.png", "icon.jpg", TestW, TestWA, TestE)));

	BEHOLDTHEWORLD.AddDenizen(*(new Denizen(100.0f, 1.25f, 100.0f, 2.0f, "demoman.png", 15.0f, 0.0f, &BEHOLDTHEWORLD, &(BEHOLDTHEWORLD.GetItemById(0)), TestD)));
	BEHOLDTHEWORLD.AddDenizen(*(new Denizen(10000.0f, 1.25f, 100.0f, 2.0f, "loh.png", 15.0f, 0.0f, &BEHOLDTHEWORLD, &(BEHOLDTHEWORLD.GetItemById(1)), TestD)));

	BEHOLDTHEWORLD.SetMap(new Map(PatternTest, 48, 48, "tileset.png"));
	MainCamera.Set(Vector2f(
		BEHOLDTHEWORLD.GetDenizenById(playerId).GetSprite().getPosition().x + BEHOLDTHEWORLD.GetDenizenById(0).GetSprite().getGlobalBounds().width / 2,
		BEHOLDTHEWORLD.GetDenizenById(playerId).GetSprite().getPosition().y + BEHOLDTHEWORLD.GetDenizenById(0).GetSprite().getGlobalBounds().height / 2),
		(Vector2f)window.getSize());

	window.setView(MainCamera.GetView());

#ifdef THREADING


	std::thread DrawThread(&Draw, std::ref(window));

	UpdateClock.restart();
	std::thread PhysicThread(&Move, std::ref(window));

	AnimUpdateClock.restart();
	std::thread AnimationThread(&Anim, std::ref(window));
#endif // THREADING
	while (window.isOpen()) {
		Event event;

		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case Event::Closed:
				window.close();
				break;
			case Event::LostFocus:
				window.setTitle("UnFocused");
				break;
			case Event::GainedFocus:
				window.setTitle("Opened");
				break;
			case Event::KeyPressed:

				switch (event.key.code) {
				case Keyboard::A:
					BEHOLDTHEWORLD.GetDenizenById(playerId).StartPreview(0, true);
					break;
				case Keyboard::S:
					BEHOLDTHEWORLD.GetDenizenById(playerId).StartPreview(1, true);
					break;
				case Keyboard::Right:
					InputAxisX += 1.0f;
					break;
				case Keyboard::Up:
					InputAxisY += 1.0f;
					break;
				case Keyboard::Left:
					InputAxisX -= 1.0f;
					break;
				case Keyboard::Down:
					InputAxisY -= 1.0f;
					break;
				case Keyboard::Num0:
					onPlayerTransition = true;
					BEHOLDTHEWORLD.GetDenizenById(playerId).MoveStateChange(0.0f, 0.0f);
					playerId = 0;
					MainCamera.Set(Vector2f(
						BEHOLDTHEWORLD.GetDenizenById(playerId).GetSprite().getPosition().x + BEHOLDTHEWORLD.GetDenizenById(0).GetSprite().getGlobalBounds().width / 2,
						BEHOLDTHEWORLD.GetDenizenById(playerId).GetSprite().getPosition().y + BEHOLDTHEWORLD.GetDenizenById(0).GetSprite().getGlobalBounds().height / 2),
						(Vector2f)window.getSize());
					window.setView(MainCamera.GetView());
					onPlayerTransition = false;
					break;
				case Keyboard::Num1:
					onPlayerTransition = true;
					BEHOLDTHEWORLD.GetDenizenById(playerId).MoveStateChange(0.0f, 0.0f);
					playerId = 1;
					MainCamera.Set(Vector2f(
						BEHOLDTHEWORLD.GetDenizenById(playerId).GetSprite().getPosition().x + BEHOLDTHEWORLD.GetDenizenById(0).GetSprite().getGlobalBounds().width / 2,
						BEHOLDTHEWORLD.GetDenizenById(playerId).GetSprite().getPosition().y + BEHOLDTHEWORLD.GetDenizenById(0).GetSprite().getGlobalBounds().height / 2),
						(Vector2f)window.getSize());
					window.setView(MainCamera.GetView());
					onPlayerTransition = false;
					break;
				default:
					break;
				}
				break;
			case Event::KeyReleased:
				switch (event.key.code) {
				case Keyboard::A:
					BEHOLDTHEWORLD.GetDenizenById(playerId).StartPreview(0, false);
					BEHOLDTHEWORLD.GetDenizenById(playerId).StartAttack(0);
#ifdef DEBUGATTACK
					std::cout << "Rej" << std::endl;
#endif // DEBUGATTACK
					break;
				case Keyboard::S:
					BEHOLDTHEWORLD.GetDenizenById(playerId).StartPreview(1, false);
					BEHOLDTHEWORLD.GetDenizenById(playerId).StartAttack(1);
#ifdef DEBUGATTACK
					std::cout << "Blow" << std::endl;
#endif // DEBUGATTACK
					break;
				case Keyboard::Right:
					InputAxisX -= 1.0f;
					break;
				case Keyboard::Up:
					InputAxisY -= 1.0f;
					break;
				case Keyboard::Left:
					InputAxisX += 1.0f;
					break;
				case Keyboard::Down:
					InputAxisY += 1.0f;
					break;
				default:
					break;
				}
				break;
			}
		}
#ifndef THREADING
		Draw(window);
		Move(window);
		Anim(window);
#endif
	}
#ifdef THREADING
	DrawThread.join();
	PhysicThread.join();
	AnimationThread.join();
#endif
	return 0;
}
void Draw(RenderWindow& window) {
#ifdef THREADING
	while (window.isOpen())
	{
#endif
		window.clear();
		for (int i = 0; i < 32; i++) {
			for (int k = 0; k < 32; k++) {
				if ((k + 2) * BEHOLDTHEWORLD.GetMap()->tileSizeX < MainCamera.GetView().getCenter().x - window.getSize().x / 2
					|| (k - 2) * BEHOLDTHEWORLD.GetMap()->tileSizeX > MainCamera.GetView().getCenter().x + window.getSize().x / 2
					|| (i + 2) * BEHOLDTHEWORLD.GetMap()->tileSizeY < MainCamera.GetView().getCenter().y - window.getSize().y / 2
					|| (i - 2) * BEHOLDTHEWORLD.GetMap()->tileSizeY > MainCamera.GetView().getCenter().y + window.getSize().y / 2) continue;
				window.draw(BEHOLDTHEWORLD.GetMap()->getTileById(k, i));
			}
		}
		for (int i = 0; i < BEHOLDTHEWORLD.GetSoulsCount(); i++) {
			/* на случай если я изобрету менее ресурсоёмкий способ
			if (BEHOLDTHEWORLD.GetDenizenById(i).GetSprite().getPosition().x + BEHOLDTHEWORLD.GetDenizenById(i).GetSprite().getTextureRect().width  < MainCamera.GetView().getCenter().x - window.getSize().x / 2
				|| BEHOLDTHEWORLD.GetDenizenById(i).GetSprite().getPosition().x> MainCamera.GetView().getCenter().x + window.getSize().x / 2
				|| BEHOLDTHEWORLD.GetDenizenById(i).GetSprite().getTextureRect().height + BEHOLDTHEWORLD.GetDenizenById(i).GetSprite().getTextureRect().height  < MainCamera.GetView().getCenter().y - window.getSize().y / 2
				|| BEHOLDTHEWORLD.GetDenizenById(i).GetSprite().getPosition().y> MainCamera.GetView().getCenter().y + window.getSize().y / 2) continue;*/
			window.draw(BEHOLDTHEWORLD.GetDenizenById(i).GetSprite());
		}
		for (int i = 0; i < BEHOLDTHEWORLD.GetItemsCount(); i++) {
			if (BEHOLDTHEWORLD.GetItemById(i).OnUse())window.draw(BEHOLDTHEWORLD.GetItemById(i).GetSprite());
		}
		for (int i = 0; i < BEHOLDTHEWORLD.GetEffectsCount(); i++) {
			window.draw(BEHOLDTHEWORLD.GetEffectById(i).GetSprite());
		}
		window.display();
#ifdef THREADING
	}
#endif
}
void Move(RenderWindow& window) {
#ifdef THREADING
	while (window.isOpen())
	{
#endif
		if (!onPlayerTransition) BEHOLDTHEWORLD.GetDenizenById(playerId).MoveStateChange(InputAxisX, InputAxisY);
		if (UpdateClock.getElapsedTime().asMilliseconds() >= 8.3 && (InputAxisX != 0 || InputAxisY != 0)) {
			BEHOLDTHEWORLD.GetDenizenById(playerId).Move(InputAxisX, InputAxisY);
			MainCamera.SetPosition(
				BEHOLDTHEWORLD.GetDenizenById(playerId).GetSprite().getPosition().x + BEHOLDTHEWORLD.GetDenizenById(0).GetSprite().getGlobalBounds().width / 2,
				BEHOLDTHEWORLD.GetDenizenById(playerId).GetSprite().getPosition().y + BEHOLDTHEWORLD.GetDenizenById(0).GetSprite().getGlobalBounds().height / 2);
			window.setView(MainCamera.GetView());
			UpdateClock.restart();
		}

#ifdef THREADING
	}
#endif
}
void Anim(RenderWindow& window) {
#ifdef THREADING
	while (window.isOpen())
	{
#endif
		for (int i = 0; i < BEHOLDTHEWORLD.GetItemsCount(); i++) {
			BEHOLDTHEWORLD.GetItemById(i).UpdateAnim();
		}
		for (int i = 0; i < BEHOLDTHEWORLD.GetSoulsCount(); i++) {
			BEHOLDTHEWORLD.GetDenizenById(i).UpdateAnim();
		}
		for (int i = 0; i < BEHOLDTHEWORLD.GetEffectsCount(); i++) {
			BEHOLDTHEWORLD.GetEffectById(i).UpdateAnim();
		}
#ifdef THREADING
	}
#endif
}
