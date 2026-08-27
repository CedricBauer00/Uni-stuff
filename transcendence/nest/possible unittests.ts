describe('get()', () => {
  it('should return group if found', async () => {
    const group = createMockGroup({ id: 'g1' });
    repo.findOneBy.mockResolvedValue(group);
    
    const result = await service.get('g1');
    
    expect(result).toEqual(new GroupsResponseDto(group));
    expect(repo.findOneBy).toHaveBeenCalledWith({ id: 'g1' });
  });
  
  it('should throw NotFoundException if not found', async () => {
    repo.findOneBy.mockResolvedValue(null);
    await expect(service.get('g1')).rejects.toThrow(NotFoundException);
  });
  
  it('should throw NotFoundException if system group', async () => {
    const group = createMockGroup({ id: 'g1', isSystem: true });
    repo.findOneBy.mockResolvedValue(group);
    await expect(service.get('g1')).rejects.toThrow(NotFoundException);
  });
});


````````````````````````````
// groups.controller.spec.ts mit comments
import { Test, TestingModule } from '@nestjs/testing';
import { GroupsController } from './groups.controller';
import { GroupsService } from './groups.service';
import { SessionAuthGuard } from '../auth/guards/session-auth.guard';

describe('GroupsController', () => {
  let controller: GroupsController;
  let service: jest.Mocked<GroupsService>; // type annotation - knows .mockResolvedValue(), .toHaveBennCalledWith(), ...

  beforeEach(async () => {
    const module: TestingModule = await Test.createTestingModule({
      controllers: [GroupsController],
      providers: [
        {
          provide: GroupsService, //wenn jmd diese aufruft
          useValue: { // bekommt instance mit mock values - actual mock
            create: jest.fn(),
            get: jest.fn(),
            findAll: jest.fn(),
            update: jest.fn(),
            deleteGroup: jest.fn(),
          },
        },
      ],
    })
      .overrideGuard(SessionAuthGuard)
      .useValue({ canActivate: jest.fn(() => true )})
      .compile();

    controller = module.get(GroupsController);
    service = module.get(GroupsService); // mock service
  });

  it('POST /groups/create calls service', async () => {
    // Arrange testdaten & Mockverhalten definiert
    const dto = { name: 'A', description: 'B' };
    const expected = { id: '1', ...dto };
    service.create.mockResolvedValue(expected);

    // act: call echter controller 
    const result = await controller.create(dto);

    // assert: check
    expect(result).toEqual(expected);
    expect(service.create).toHaveBeenCalledWith(dto);
  });

});
```````````