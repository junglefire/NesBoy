#ifndef EMULATOR_H
#define EMULATOR_H

#include "PictureBus.h"
#include "Controller.h"
#include "MainBus.h"
#include "Define.h"
#include "CPU.h"
#include "PPU.h"

NAMESPACE_BEGIN(sn)

const int NESVideoWidth = ScanlineVisibleDots;
const int NESVideoHeight = VisibleScanlines;

class Emulator {
public:
	Emulator();
	void run(std::string rom_path);
	void setVideoWidth(int width);
	void setVideoHeight(int height);
	void setVideoScale(float scale);
	void setKeys(std::vector<sf::Keyboard::Key>& p1, std::vector<sf::Keyboard::Key>& p2);
private:
	void DMA(Byte page);
	MainBus m_bus;
	PictureBus m_pictureBus;
	CPU m_cpu;
	PPU m_ppu;
	Cartridge m_cartridge;
	std::unique_ptr<Mapper> m_mapper;
	Controller m_controller1, m_controller2;
	sf::RenderWindow m_window;
	VirtualScreen m_emulatorScreen;
	float m_screenScale;
	TimePoint m_cycleTimer;
	Duration m_elapsedTime;
	NanoSeconds m_cpuCycleDuration;
};

NAMESPACE_END(sn)

#endif // EMULATOR_H
